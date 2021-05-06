import os
import tensorflow as tf
import numpy as np
import random

os.environ['CUDA_VISIBLE_DEVICES'] = '0'


def load_moving_mnist():
    data = np.load('mnist_test_seq.npy')
    data = np.transpose(data, [1, 2, 3, 0])
    return data


def autoencoder_model(previous_seq):
    conv = tf.layers.conv2d(previous_seq, filters=32, kernel_size=(3, 3),
                            strides=(2, 2), padding='SAME',
                            activation=tf.nn.relu)
    conv_temp_1 = tf.layers.conv2d(conv, filters=32, kernel_size=(3, 3),
                                   strides=(1, 1), padding='SAME',
                                   activation=tf.nn.relu)
    conv = tf.layers.conv2d(conv_temp_1, filters=64, kernel_size=(3, 3),
                            strides=(2, 2), padding='SAME',
                            activation=tf.nn.relu)
    conv_temp_2 = tf.layers.conv2d(conv, filters=64, kernel_size=(3, 3),
                                   strides=(1, 1), padding='SAME',
                                   activation=tf.nn.relu)
    conv = tf.layers.conv2d(conv_temp_2, filters=128, kernel_size=(3, 3),
                            strides=(2, 2), padding='SAME',
                            activation=tf.nn.relu)
    conv = tf.layers.conv2d(conv, filters=128, kernel_size=(3, 3),
                            strides=(1, 1), padding='SAME',
                            activation=tf.nn.relu)

    deconv = tf.layers.conv2d_transpose(conv, filters=128, kernel_size=(3, 3),
                                        strides=(1, 1), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.layers.conv2d_transpose(deconv, filters=64, kernel_size=(3, 3),
                                        strides=(2, 2), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.concat([deconv, conv_temp_2], axis=3)
    deconv = tf.layers.conv2d_transpose(deconv, filters=64, kernel_size=(3, 3),
                                        strides=(1, 1), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.layers.conv2d_transpose(deconv, filters=32, kernel_size=(3, 3),
                                        strides=(2, 2), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.concat([deconv, conv_temp_1], axis=3)
    deconv = tf.layers.conv2d_transpose(deconv, filters=32, kernel_size=(3, 3),
                                        strides=(1, 1), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.layers.conv2d_transpose(deconv, filters=1, kernel_size=(3, 3),
                                        strides=(2, 2), padding='SAME',
                                        activation=tf.nn.relu)

    tf.summary.image('input_frame_1', previous_seq[:, :, :, 0:1], 3)
    tf.summary.image('input_frame_2', previous_seq[:, :, :, 1:2], 3)
    tf.summary.image('input_frame_3', previous_seq[:, :, :, 2:3], 3)
    tf.summary.image('predicted_next_frame', deconv[:, :, :, 0:1], 3)

    return deconv


def main():
    previous_num = 3
    batch_size = 128
    lr = .0002
    bta1 = .9
    bta2 = .999
    eps = .00000001

    batch_seq = tf.placeholder(tf.float32, shape=[None, 64, 64,
                                                  previous_num + 1])
    previous_seq = batch_seq[:, :, :, 0:previous_num]
    gt_next_frame = batch_seq[:, :, :, previous_num: previous_num + 1]
    predicted_next_frame = autoencoder_model(previous_seq)

    tf.summary.image('gt_next_frame', gt_next_frame[:, :, :, 0:1], 3)

    with tf.name_scope('Loss'):
        pixel_loss = tf.reduce_mean(tf.square(predicted_next_frame -
                                              gt_next_frame))
        tf.summary.scalar('pixel_loss', pixel_loss)

    optimizer = tf.train.AdamOptimizer(learning_rate=lr,
                                       beta1=bta1, beta2=bta2,
                                       epsilon=eps).minimize(pixel_loss)
    summ = tf.summary.merge_all()

    saver = tf.train.Saver()

    with tf.Session() as sess:
        LOGDIR = './moving_mnist_video_tutorial/'
        test_writer = tf.summary.FileWriter(LOGDIR + '/Test')

        data = load_moving_mnist()

        test_index = range(0, 3)
        test_data = data[test_index, :, :, :]

        train_index = range(0, 10000)
        train_index = [x for x in train_index if x not in test_index]
        train_data = data[train_index, :, :, :]

        sess.run(tf.global_variables_initializer())
        for itr in range(20001):
            batch_idx = random.sample(range(train_data.shape[0]), batch_size)
            seq_idx = random.randint(0, 20 - (previous_num + 1))
            train_batch = train_data[batch_idx, :, :, seq_idx: seq_idx
                                                               + previous_num
                                                               + 1]
            sess.run([optimizer], feed_dict={batch_seq: train_batch})
            if itr % 100 == 0:
                [train_loss] = sess.run([pixel_loss],
                                        feed_dict={batch_seq: train_batch})
                feed_dict = {batch_seq: test_data[:, :, :, 0: previous_num
                                                              + 1]}
                [test_loss, s] = sess.run([pixel_loss, summ],
                                          feed_dict=feed_dict)
                print('@ iteration: %i, Training loss = %.6f, Test loss = %.6f'
                      % (itr, train_loss, test_loss))
                test_writer.add_summary(s, itr)
                saver.save(sess, os.path.join(LOGDIR, 'model.ckpt'), itr)


if __name__ == '__main__':
    main()
