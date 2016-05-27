## Pretty much ripped straight from the pycaffe example.
from __future__ import print_function
from caffe import layers as L, params as P, to_proto
from caffe.proto import caffe_pb2
import getopt, sys
## helper function for common structures

# layer types (we only use a convolution followed by a simple (positive) rectification, maximization over an input pool from such a layer, and fully connected layers)

# the loss layer is defined inside of the net; here we use softmax but I may try something else

def conv_relu(bottom, ks, nout, stride=1, pad=0, group=1):
    conv = L.Convolution(bottom, kernel_size=ks, stride=stride,
                                num_output=nout, pad=pad, group=group)
    return conv, L.ReLU(conv, in_place=True)

def fc_relu(bottom, nout):
    fc = L.InnerProduct(bottom, num_output=nout)
    return fc, L.ReLU(fc, in_place=True)

def max_pool(bottom, ks, stride=1):
    return L.Pooling(bottom, pool=P.Pooling.MAX, kernel_size=ks, stride=stride)

# an actual network

def caffenet(lmdb, batch_size=256, include_acc=False):
    data, label = L.Data(source=lmdb, backend=P.Data.LMDB, batch_size=batch_size, ntop=2,
        transform_param=dict(crop_size=227, mean_value=[104, 117, 123], mirror=True))

    # the net itself
    # convolve adjacent input pixels. Output image is of size 8
    conv1, relu1 = conv_relu(data, 3, 8)
    # pools over relu1, the rectified convolutions
    pool1 = max_pool(relu1, 2)
    #norm1 = L.LRN(pool1, local_size=5, alpha=1e-4, beta=0.75)
    #conv2, relu2 = conv_relu(norm1, 5, 256, pad=2, group=2)
    #pool2 = max_pool(relu2, 3, stride=2)
    #norm2 = L.LRN(pool2, local_size=5, alpha=1e-4, beta=0.75)
    #conv3, relu3 = conv_relu(norm2, 3, 384, pad=1)
    #conv4, relu4 = conv_relu(relu3, 3, 384, pad=1, group=2)
    #conv5, relu5 = conv_relu(relu4, 3, 256, pad=1, group=2)
    #pool5 = max_pool(relu5, 3, stride=2)
    fc6, relu6 = fc_relu(pool1, 64)
    drop6 = L.Dropout(relu6, in_place=True)
    #fc7, relu7 = fc_relu(drop6, 4096)
    #drop7 = L.Dropout(relu7, in_place=True)
    fc8 = L.InnerProduct(drop6, num_output=64)
    loss = L.SoftmaxWithLoss(fc8, label)

    if include_acc:
        acc = L.Accuracy(fc8, label)
        return to_proto(loss, acc)
    else:
        return to_proto(loss)

# now make the net using the above methods
def make_net(inputs):
    trainingNet = ''
    testNet = ''
    try:
        opts, args = getopt.getopt(inputs,"hi:o:",["ifile=","ofile="])
    except getopt.GetoptError:
        print('caffeTest.py -train <trainingNet> -test <testNet>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('test.py -i <trainingdata> -o <testdata>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            trainingNet = arg
        elif opt in ("-o", "--ofile"):
            testNet = arg
    print('Training net is ---',trainingNet,'---')
    print('Test net is ---', testNet,'---')
    with open('trainNet', 'w') as f:
 		print(caffenet(trainingNet), file=f)
    with open('testNet', 'w') as f:
        print(caffenet(testNet, batch_size=50, include_acc=True), file=f)

if __name__ == '__main__':
    make_net(sys.argv[1:])