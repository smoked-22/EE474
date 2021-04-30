import os
import tensorflow as tf
import pickle
import numpy as np
import random

os.environ['CUDA_VISIBLE_DEVICES'] = '0'


def load_cifar10():
    data_dir = './cifar-10-batches-py'

    data_list = []
    for i in range(1, 5 + 1):
        data_sub_dir = data_dir + '/data_batch_' + str(i)
        with open(data_sub_dir, 'rb') as fo:
            data_list.append(pickle.load(fo)['data'])

    data = np.concatenate(data_list, axis=0)
    # data = np.concatenate(data1, data2, data3, data4, data5, axis=0)
    data = np.reshape(data, [50000, 3, 32, 32])
    data = np.transpose(data, [0, 2, 3, 1])
    return data


def autoencoder_model(rgb_im):
    conv1 = tf.layers.conv2d(rgb_im, filters=32,
                             kernel_size=(3, 3),
                             strides=(2, 2),
                             padding='SAME',
                             activation=tf.nn.relu,
                             name='Conv_1')
    conv2 = tf.layers.conv2d(conv1, filters=64,
                             kernel_size=(3, 3),
                             strides=(2, 2),
                             padding='SAME',
                             activation=tf.nn.relu,
                             name='Conv_2')
    deconv1 = tf.layers.conv2d_transpose(conv2, filters=32,
                                         kernel_size=(3, 3),
                                         strides=(2, 2),
                                         padding='SAME',
                                         activation=tf.nn.relu,
                                         name='Deconv_1')
    deconv2 = tf.layers.conv2d_transpose(deconv1, filters=3,
                                         kernel_size=(3, 3),
                                         strides=(2, 2),
                                         padding='SAME',
                                         activation=None,
                                         name='Deconv_2')

    tf.summary.image('input', rgb_im, 3)

    gt_yuv_im = tf.image.rgb_to_yuv(rgb_im)
    tf.summary.image('gt_Y', gt_yuv_im[:, :, :, 0:1], 3)
    tf.summary.image('gt_U', gt_yuv_im[:, :, :, 1:2], 3)
    tf.summary.image('gt_V', gt_yuv_im[:, :, :, 2:3], 3)

    tf.summary.image('predicted_Y', deconv2[:, :, :, 0:1], 3)
    tf.summary.image('predicted_U', deconv2[:, :, :, 1:2], 3)
    tf.summary.image('predicted_V', deconv2[:, :, :, 2:3], 3)

    return deconv2


def main():
    batch_size = 128
    lr = 2e-4
    bta1 = .9
    bta2 = .999
    eps = 1e-8

    rgb_im = tf.placeholder(tf.float32, shape=[None, 32, 32, 3], name='x')
    predicted_yuv_im = autoencoder_model(rgb_im)

    with tf.name_scope('Loss'):
        pixel_loss = tf.reduce_mean(tf.square(predicted_yuv_im -
                                              tf.image.rgb_to_yuv(rgb_im)))
        tf.summary.scalar('pixel_loss', pixel_loss)

        optimizer = tf.train.AdamOptimizer(learning_rate=lr,
                                           beta1=bta1,
                                           beta2=bta2,
                                           epsilon=eps).minimize(
            pixel_loss)

        summ = tf.summary.merge_all()
        saver = tf.train.Saver()



if __name__ == '__main__':
    main()
