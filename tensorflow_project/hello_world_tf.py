import tensorflow as tf
import numpy as np
import os

os.environ['CUDA_VISIBLE_DEVICES'] = '0'

def main():
    x_data = np.random.rand(100).astype(np.float32)
    y_data = .1 * x_data + .3

    W = tf.Variable(tf.random_uniform([1], minval=-1, maxval=1))
    b = tf.Variable(tf.zeros([1]))
    y = W * x_data + b

    MSE_loss = tf.reduce_mean(tf.square(y - y_data), name='MSE_Loss')
    optimizer = tf.train.GradientDescentOptimizer(learning_rate=.5)
    train = optimizer.minimize(MSE_loss)

    with tf.Session() as sess:
        sess.run(tf.global_variables_initializer())
        for itr in range(201):
            sess.run(train)
            if itr % 10 == 0:
                print('@ iteration %i, estimated W = %.5f, estimated b = %.5f'% (itr, sess.run(W), sess.run(b)))

        print('#' * 100)
        print('Learning Complete and estimated parameters [ W, b ] = [%.5f, %.5f]' % (sess.run(W), sess.run(b)))
        print('#' * 100)


if __name__ == '__main__':
    main()
