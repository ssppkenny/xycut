import cv2
import numpy as np
import time
from operator import itemgetter


class ImageNode:
    def __init__(self,image):
        self.__image = image
        self.__left = None
        self.__right = None
    def left(self, left):
        self.__left = left
    def right(self,right):
        self.__right = right

    def left_(self):
        return self.__left

    def right_(self):
        return self.__right

    def to_list(self):
        out_list = []
        if self.__left is None and self.__right is None:
            return [self.__image]
        if not self.__left is None:
            out_list += self.__left.to_list()
        if not self.__right is None:
            out_list += self.__right.to_list()
        return out_list




def zero_runs(a):
    # Create an array that is 1 where a is 0, and pad each end with an extra 0.
    iszero = np.concatenate(([0], np.equal(a, 0).view(np.int8), [0]))
    absdiff = np.abs(np.diff(iszero))
    # Runs start and end where absdiff is 1.
    ranges = np.where(absdiff == 1)[0].reshape(-1, 2)
    return ranges


def image_whithout_white_borders(image):
    """
    removes empty borders from image
    and returns image wothout empty borders
    :param image:
    :return:
    """
    img = np.copy(image)
    cv2.bitwise_not(img, img)
    h_prof = np.sum(image, axis=0) ##horizonal
    v_prof = np.sum(image, axis=1) ##vertical
    h,w = img.shape
    h_nonzero = np.nonzero(h_prof)
    v_nonzero = np.nonzero(v_prof)
    left = h_nonzero[0][0]
    right = h_nonzero[0][len(h_nonzero[0])-1]
    upper = v_nonzero[0][0]
    lower = v_nonzero[0][len(v_nonzero[0])-1]
    return image[upper:lower,left:right]


def max_ind(zr):
    gaps = list(map(lambda x: x[1] - x[0], zr))
    gaps_with_index = list(enumerate(gaps))
    gaps_with_index = list(filter(lambda x: x[1] > threshold, gaps_with_index))
    if len(gaps_with_index) >0:
        maxind = max(gaps_with_index, key=itemgetter(1))[0]
        return maxind
    else:
        return -1


def xycut_step(image, threshold, tree):
    h_prof = np.sum(image, axis=0)  ##horizonal
    zr = zero_runs(h_prof)
    maxind = max_ind(zr)

    cuts_done = 0

    if maxind >= 0:
        _from, _to = zr[maxind]
        left_image = image[:, :_from]
        right_image = image[:, _to:]
        (h, w) = left_image.shape
        if h > 0 and w > 0:
            cuts_done += 1
            left_tree = ImageNode(left_image)
            tree.left(left_tree)
            xycut_step(left_image, threshold, left_tree)
        (h, w) = right_image.shape
        if h > 0 and w > 0:
            cuts_done += 1
            right_tree = ImageNode(right_image)
            tree.right(right_tree)
            xycut_step(right_image, threshold, right_tree)


    else:
        v_prof = np.sum(image, axis=1)  ##vertical
        zr = zero_runs(v_prof)
        maxind = max_ind(zr)
        if maxind >= 0:
            _from, _to = zr[maxind]
            upper_image = image[:_from, :]
            lower_image = image[_to:, :]
            (h,w) = upper_image.shape
            if h>0 and w>0:
                cuts_done += 1
                upper_tree = ImageNode(upper_image)
                tree.left(upper_tree)
                xycut_step(upper_image, threshold, upper_tree)
            (h, w) = lower_image.shape
            if h > 0 and w > 0:
                cuts_done += 1
                lower_tree = ImageNode(lower_image)
                tree.right(lower_tree)
                xycut_step(lower_image, threshold, lower_tree)


if __name__ == '__main__':
    input_file = '2019-04-6.png'
    gray = cv2.imread(input_file, 0)

    ## calculate font size
    cv2.bitwise_not(gray, gray)
    ret2, gray = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    num_labels, labels, stats, centroids = cv2.connectedComponentsWithStats(gray, 8, cv2.CV_32S)

    heights = []

    for i in range(1, num_labels):
        heights.append(stats[i, cv2.CC_STAT_HEIGHT])

    gray = cv2.imread(input_file, 0)

    threshold = np.mean(heights)
    print(threshold)

    ret2, th2 = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    cv2.bitwise_not(th2, th2)
    kernel = np.ones((5, 5), np.uint8)
    closing = cv2.morphologyEx(th2, cv2.MORPH_CLOSE, kernel)

    closing = image_whithout_white_borders(closing)

    image_counter = 0

    image_tree = ImageNode(closing)
    xycut_step(closing, threshold, image_tree)
    l=image_tree.to_list()
    print(len(l))
    for i in range(len(l)):
        cv2.imwrite("image" + str(i) + ".png", l[i])


