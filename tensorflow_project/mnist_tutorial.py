import os
import tensorflow as tf

os.environ['CUDA_VISIBLE_DEVICES'] = '1'

LOGDIR = './mnist_tutorial/'

mnist = tf.contrib.learn.datasets.mnist.read_data_sets(
    train_dir=LOGDIR + 'data', one_hot=True)


def conv_layer(x_in, conv_kernel, conv_strides, conv_padding='SAME',
               name='Conv'):
    with tf.name_scope(name):
        w = tf.Variable(tf.truncated_normal(conv_kernel, mean=.0, stddev=.1),
                        name='W')
        b = tf.Variable(tf.constant(.0, shape=[conv_kernel[3]]), name='b')

        conv = tf.nn.conv2d(x_in, w, strides=conv_strides,
                            padding=conv_padding)

        act = tf.nn.relu(conv + b)

        tf.summary.histogram('weights', w)
        tf.summary.histogram('biases', b)
        tf.summary.histogram('activations', act)

        return act


def pool_layer(x_in, poo_kernel, pool_strides, poo_padding='VALID',
               name='Pool'):
    with tf.name_scope(name):
        pool = tf.nn.max_pool(x_in, ksize=poo_kernel, strides=pool_strides,
                              padding=poo_padding)
        return pool


def fc_layer(x_in, size_in, size_out, dropoutProb=None, name='FC'):
    with tf.name_scope(name):
        w = tf.Variable(tf.truncated_normal([size_in, size_out], stddev=.1),
                        name='W')
        b = tf.Variable(tf.constant(.1, shape=[size_out]), name='b')

        act = tf.nn.relu(tf.matmul(x_in, w) + b)

        if dropoutProb is not None:
            act = tf.nn.dropout(act, dropoutProb, name='dropout')

        tf.summary.histogram('weights', w)
        tf.summary.histogram('biases', b)
        tf.summary.histogram('activations', act)
        return act


def bn_layer(x_in, offset=None, scale=None,
             name='bn'):
    with tf.name_scope(name):
        mean, variance = tf.nn.moments(x_in, axis=[0], keep_dims=True)
        # epsilon set arbitrarily
        batch_norm = tf.nn.batch_normalization(x=x_in, mean=mean,
                                               variance=variance,
                                               offset=offset,
                                               scale=scale,
                                               variance_epsilon=1e-7)
        return batch_norm


def CNN_model(batch_in, n_classes, nShape, nChannels, dropout):
    batch_in = tf.reshape(batch_in, [-1, nShape, nShape, nChannels])
    tf.summary.image('input', batch_in, 10)

    batch_in = conv_layer(batch_in, [3, 3, nChannels, 32], [1, 1, 1, 1], name='Conv_1')
    batch_in = bn_layer(batch_in, name='BN_1')
    batch_in = pool_layer(batch_in, [1, 2, 2, 1], [1, 2, 2, 1], name='Pool_1')

    batch_in = conv_layer(batch_in, [3, 3, 32, 64], [1, 1, 1, 1], name='Conv_2')
    batch_in = bn_layer(batch_in, name='BN_2')
    batch_in = pool_layer(batch_in, [1, 2, 2, 1], [1, 2, 2, 1], name='Pool_2')

    batch_in_shape = batch_in.get_shape().as_list()
    flattened = tf.reshape(batch_in,
                           [-1,
                            batch_in_shape[1] * batch_in_shape[2] * batch_in_shape[3]])

    flattened_shape = flattened.get_shape().as_list()
    fc1 = fc_layer(flattened, flattened_shape[1], 1024,
                   dropoutProb=dropout, name='FC_1')
    fc2 = fc_layer(fc1, 1024, 512, dropoutProb=dropout, name='FC_2')

    Prediction = fc_layer(fc2, 512, n_classes, name='Predicted_Digit')
    return Prediction


def main():
    minibatch_size = 128
    lear_rate = .0001
    bta1 = .9
    bta2 = .999
    epsln = 1e-8

    keep_prob = tf.placeholder(tf.float32, name='dropout_keep_prob')
    ph_x = tf.placeholder(tf.float32, shape=[None, 784], name='x')
    ph_y = tf.placeholder(tf.float32, shape=[None, 10], name='labels')

    Predicted_Digit_logits = CNN_model(ph_x, n_classes=10, nShape=28,
                                       nChannels=1, dropout=keep_prob)

    with tf.name_scope('Loss'):
        Digit_Softmax_loss = tf.reduce_mean(
            tf.nn.softmax_cross_entropy_with_logits(
                logits=Predicted_Digit_logits,
                labels=ph_y,
                name='Digit_Softmax_loss'))
        tf.summary.scalar('Digit_Softmax_loss', Digit_Softmax_loss)
    optimizer = tf.train.AdamOptimizer(learning_rate=lear_rate,
                                       beta1=bta1,
                                       beta2=bta2,
                                       epsilon=epsln).minimize(
        Digit_Softmax_loss)

    with tf.name_scope('Digit_Classification_Accuracy'):
        output_digit_softmax = tf.nn.softmax(Predicted_Digit_logits,
                                             name='output_digit_softmax')
        Predicted_Digit = tf.cast(tf.argmax(output_digit_softmax, 1), tf.int32)
        Label = tf.cast(tf.argmax(ph_y, 1), tf.int32)
        Correct_Class = tf.equal(Predicted_Digit, Label)
        Classification_accuracy = tf.reduce_mean(
            tf.cast(Correct_Class, tf.float32))
        tf.summary.scalar('Digit_Classification_Acc', Classification_accuracy)

    summ = tf.summary.merge_all()

    saver = tf.train.Saver()

    with tf.Session() as sess:
        version = 0
        while os.path.isdir(os.path.join(LOGDIR, 'version_' + str(version))):
            version += 1
        Train_writer = tf.summary.FileWriter(
            os.path.join(LOGDIR, 'version_' + str(version), 'Train'),
            sess.graph)
        Test_writer = tf.summary.FileWriter(
            os.path.join(LOGDIR, 'version_' + str(version), 'Test'))

        sess.run(tf.global_variables_initializer())
        for itr in range(2001):
            batch = mnist.train.next_batch(batch_size=minibatch_size)
            _ = sess.run([optimizer],
                         feed_dict={ph_x: batch[0], ph_y: batch[1],
                                    keep_prob: .5})
            if itr % 10 == 0:
                [train_acc, s] = sess.run([Classification_accuracy, summ],
                                          feed_dict={ph_x: batch[0],
                                                     ph_y: batch[1],
                                                     keep_prob: 1.})
                Train_writer.add_summary(s, itr)
                [test_acc, s] = sess.run([Classification_accuracy, summ],
                                         feed_dict={ph_x: mnist.test.images,
                                                    ph_y: mnist.test.labels,
                                                    keep_prob: 1.})
                print('@ iteration: %i, Training Acc. = %.6f, Test Acc. = '
                      '%.6f ' % (itr, train_acc, test_acc))
                Test_writer.add_summary(s, itr)
                saver.save(sess, os.path.join(LOGDIR, 'model.ckpt'), itr)

    return 1


if __name__ == '__main__':
    main()
