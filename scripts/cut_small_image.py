"""
useage set sets and classes
then ..
"""

import xml.etree.ElementTree as ET
import pickle
import os
from os import listdir, getcwd
from os.path import join
from PIL import Image

sets=[('2007', 'train')]

classes = ["aeroplane", "bicycle", "bird", "boat", "bottle", "bus", "car", "cat", "chair", "cow", "diningtable", "dog", "horse", "motorbike", "person", "pottedplant", "sheep", "sofa", "train", "tvmonitor"]

def convert(box):
    x = (box[0] + box[1])/2.0 - 1
    y = (box[2] + box[3])/2.0 - 1
    w = box[1] - box[0]
    h = box[3] - box[2]
    return (x,y,w,h)

def cut_images(year, image_id,list_file):
    in_file = open('VOCdevkit/VOC%s/Annotations/%s.xml'%(year, image_id))
    im = Image.open('VOCdevkit/VOC%s/JPEGImages/%s.jpg'%(year, image_id))

    tree=ET.parse(in_file)
    root = tree.getroot()
    size = root.find('size')
    image_inner_id = 0
    for obj in root.iter('object'):
        cls = obj.find('name').text
        if cls not in classes:
            continue
        image_inner_id = image_inner_id + 1
        xmlbox = obj.find('bndbox')
        b = (float(xmlbox.find('xmin').text), float(xmlbox.find('xmax').text), float(xmlbox.find('ymin').text), float(xmlbox.find('ymax').text))
        bb = convert(b)
        x = b[0]
        y = b[2]
        w = bb[2]
        h = bb[3]
        region = im.crop((x, y, x+w, y+h))
        region.save('%s/VOCdevkit/VOC%s/labels_img/%s_%d.jpg'%(wd, year, image_id, image_inner_id))
        list_file.write('%s %s/VOCdevkit/VOC%s/labels_img/%s_%d.jpg\n'%(cls,wd, year, image_id, image_inner_id))

wd = getcwd()

for year, image_set in sets:
    if not os.path.exists('VOCdevkit/VOC%s/labels_img/'%(year)):
        os.makedirs('VOCdevkit/VOC%s/labels_img/'%(year))
    image_ids = open('VOCdevkit/VOC%s/ImageSets/Main/%s.txt'%(year, image_set)).read().strip().split()
    list_file = open('%s_%s.txt'%(year, image_set), 'w')
    for image_id in image_ids:
        cut_images(year, image_id, list_file)
    list_file.close()
