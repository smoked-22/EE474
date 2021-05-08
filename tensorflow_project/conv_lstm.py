import tensorflow as tf


class Unit(object):
    def __init__(self, shape, kernel_size, filters):
        self.shape = shape
        self.kernel_size = kernel_size
        self.filters = filters

    def __call__(self, inputs, h, c):
        with tf.variable_scope('conv_input'):
            inputs_after_conv = tf.layers.conv2d(inputs, 4 * self.filters,
                                                 self.kernel_size, (1, 1),
                                                 'SAME')
        with tf.variable_scope('conv_hidden'):
            hidden_after_conv = tf.layers.conv2d(h, 4 * self.filters,
                                                 self.kernel_size, (1, 1),
                                                 'SAME')

        xi, xc, xf, xo = tf.split(inputs_after_conv, 4, axis=3)
        hi, hc, hf, ho = tf.split(hidden_after_conv, 4, axis=3)

        it = tf.nn.sigmoid(xi + hi)
        ft = tf.nn.sigmoid(xf + hf)

        new_c = (ft * c) + (it * tf.nn.tanh(xc + hc))
        ot = tf.nn.sigmoid(xo + ho)
        new_h = ot * tf.nn.tanh(new_c)

        return new_h, new_c
