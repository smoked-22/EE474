import os
import tensorflow as tf
import numpy as np
import random
import conv_lstm

# os.environ['CUDA_VISIBLE_DEVICES'] = '0'


def encoder(x):
    conv = tf.layers.conv2d(x, filters=32, kernel_size=(3, 3), strides=(2, 2),
                            padding='SAME', activation=tf.nn.relu)
    conv = tf.layers.conv2d(conv, filters=32, kernel_size=(3, 3),
                            strides=(1, 1),
                            padding='SAME', activation=tf.nn.relu)
    conv = tf.layers.conv2d(conv, filters=64, kernel_size=(3, 3),
                            strides=(2, 2),
                            padding='SAME', activation=tf.nn.relu)
    conv = tf.layers.conv2d(conv, filters=64, kernel_size=(3, 3),
                            strides=(1, 1),
                            padding='SAME', activation=tf.nn.relu)
    return conv


def decoder(x):
    deconv = tf.layers.conv2d_transpose(x, filters=64, kernel_size=(3, 3),
                                        strides=(1, 1), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.layers.conv2d_transpose(deconv, filters=32, kernel_size=(3, 3),
                                        strides=(2, 2), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.layers.conv2d_transpose(deconv, filters=32, kernel_size=(3, 3),
                                        strides=(1, 1), padding='SAME',
                                        activation=tf.nn.relu)
    deconv = tf.layers.conv2d_transpose(deconv, filters=1, kernel_size=(3, 3),
                                        strides=(2, 2), padding='SAME',
                                        activation=tf.nn.relu)
    return deconv


def predictor_model(previous_seq, previous_num, prediction_num):
    frame_height = previous_seq.get_shape().as_list()[1]
    frame_width = previous_seq.get_shape().as_list()[2]
    encoder_input = tf.transpose(previous_seq, perm=[0, 3, 1, 2])
    encoder_input = tf.reshape(encoder_input, [-1, frame_height, frame_width])
    encoder_input = tf.expand_dims(encoder_input, axis=3)

    enc_feature = encoder(encoder_input)
    feature_height = enc_feature.get_shape().as_list()[1]
    feature_width = enc_feature.get_shape().as_list()[2]
    feature_channel = enc_feature.get_shape().as_list()[3]

    enc_feature = tf.reshape(enc_feature, [-1, previous_num, feature_height,
                                           feature_width, feature_channel])

    conv_lstm_h1 = tf.zeros_like(enc_feature[:, 0, :, :, :])
    conv_lstm_c1 = tf.zeros_like(enc_feature[:, 0, :, :, :])
    conv_lstm_h2 = tf.zeros_like(enc_feature[:, 0, :, :, :])
    conv_lstm_c2 = tf.zeros_like(enc_feature[:, 0, :, :, :])

    unit1 = conv_lstm.Unit(shape=[feature_height, feature_width],
                           kernel_size=[5, 5], filters=64)
    unit2 = conv_lstm.Unit(shape=[feature_height, feature_width],
                           kernel_size=[5, 5], filters=64)

    predictions = []
    for seq_i in range(previous_num + prediction_num - 1):
        if seq_i > previous_num - 1:
            inputs = tf.zeros_like(enc_feature[:, 0, :, :, :])
        else:
            inputs = enc_feature[:, seq_i, :, :, :]
        with tf.variable_scope('convlstm_1', reuse=tf.AUTO_REUSE):
            conv_lstm_h1, conv_lstm_c1 = unit1(inputs=inputs,
                                               h=conv_lstm_h1, c=conv_lstm_c1)
        with tf.variable_scope('convlstm_2', reuse=tf.AUTO_REUSE):
            conv_lstm_h2, conv_lstm_c2 = unit2(inputs=conv_lstm_h1,
                                               h=conv_lstm_h2, c=conv_lstm_c2)
        if seq_i > previous_num - 2:
            predictions.append(decoder(conv_lstm_h2))

    # prediction = decoder(conv_lstm_h2)
    # tf.summary.image('input_frame_3', previous_seq[:, :, :, 2:3], 3)
    # tf.summary.image('input_frame_4', previous_seq[:, :, :, 3:4], 3)
    # tf.summary.image('input_frame_5', previous_seq[:, :, :, 4:5], 3)
    # tf.summary.image('predicted_next_frame', prediction[:, :, :, :], 3)

    # return prediction

    for i in range(previous_num):
        tf.summary.image('input_frame_' + str(i + 1), previous_seq[:, :, :, i:i+1],
                         3)
    for i in range(prediction_num):
        tf.summary.image('predicted_frame_' + str(i + 1), predictions[i], 3)

    return tf.concat(predictions, -1)


def main():
    previous_num = 5
    prediction_num = 3
    batch_size = 128
    lr = 2e-4
    bta1 = .9
    bta2 = .999
    eps = 1e-7

    batch_seq = tf.placeholder(tf.float32,
                               shape=[None, 64, 64,
                                      previous_num + prediction_num])
    previous_seq = batch_seq[:, :, :, 0: previous_num]
    gt_next_frames = batch_seq[:, :, :,
                     previous_num: previous_num + prediction_num]
    # predicted_next_frame = predictor_model(previous_seq, previous_num,
    #                                        prediction_num)
    pred_next_frames = predictor_model(previous_seq, previous_num,
                                       prediction_num)
    for i in range(prediction_num):
        tf.summary.image('gt_next_frame_' + str(i+1),
                         gt_next_frames[:, :, :, i:i+1], 3)

    with tf.name_scope('Loss'):
        for i in range(prediction_num):
            pixel_loss = tf.reduce_mean(tf.square(pred_next_frames -
                                                  gt_next_frames))
        tf.summary.scalar('pixel_loss', pixel_loss)

    optimizer = tf.train.AdamOptimizer(learning_rate=lr,
                                       beta1=bta1, beta2=bta2,
                                       epsilon=eps).minimize(pixel_loss)
    summ = tf.summary.merge_all()

    saver = tf.train.Saver()

    with tf.Session() as sess:
        LOGDIR = './moving_mnist_video_tutorial2/'
        version = 0
        while os.path.isdir(os.path.join(LOGDIR, 'Test_' + str(version))):
            version += 1
        test_writer = tf.summary.FileWriter(LOGDIR + 'Test_' + str(version))

        data = load_moving_mnist() / 1.

        test_index = range(0, 3)
        test_data = data[test_index, :, :, :]

        train_index = range(0, 10000)
        train_index = [x for x in train_index if x not in test_index]
        train_data = data[train_index, :, :, :]

        sess.run(tf.global_variables_initializer())
        for itr in range(20001):
            batch_idx = random.sample(range(train_data.shape[0]), batch_size)
            seq_idx = random.randint(0, 20 - (previous_num + prediction_num))
            train_batch = train_data[batch_idx, :, :, seq_idx: seq_idx
                                                               + previous_num
                                                               + prediction_num]
            sess.run([optimizer], feed_dict={batch_seq: train_batch})
            if itr % 100 == 0:
                [train_loss] = sess.run([pixel_loss],
                                        feed_dict={batch_seq: train_batch})
                feed_dict = {batch_seq: test_data[:, :, :, 0: previous_num
                                                              + prediction_num]}
                [test_loss, s] = sess.run([pixel_loss, summ],
                                          feed_dict=feed_dict)
                print('@ iteration: %i, Training loss = %.6f, Test loss = %.6f'
                      % (itr, train_loss, test_loss))
                test_writer.add_summary(s, itr)
                saver.save(sess, os.path.join(LOGDIR + 'Test_' + str(version),
                                              'model.ckpt'),
                           itr)


if __name__ == '__main__':
    main()
