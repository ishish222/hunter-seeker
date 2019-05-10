#!/usr/bin/env python

from lxml import objectify, etree
import re
import random
import sys

if(len(sys.argv) < 2):
    print('Give a filename')
    exit(1)

input_ = sys.argv[1]

f = open(input_, 'rb')
xml_data = f.read()
f.close()

tree = etree.fromstring(xml_data)

data = ''

def convert_xml(data):
    #
    current_node = None
    diving_chunks = data.split('>')
    for dchunk in diving_chunks:
#        print('Parsing dchunk: {}'.format(dchunk))
        if(len(dchunk) == 0): 
            continue
        if(current_node == None):
            current_node = etree.Element('child')
            root = current_node
        else:
            child = etree.Element('child')
            current_node.append(child)
            current_node = child
        #
        surfacing_chunks = dchunk.split('<')
        for schunk in surfacing_chunks:
#            print('Parsing schunk: {}'.format(schunk))
            if(len(schunk) > 0):
                attrs = re.findall(r'(?:[^\s,"]|"(?:\\.|[^"])*")+', schunk)
#                print('Attrs: {}'.format(attrs))
                try:
                    tag = attrs.pop(0)
                except IndexError:
                    continue
#                print('Setting tag: {}'.format(tag))
                try:
                    current_node.tag = tag
                except ValueError:
                    tag = ''.join(e for e in tag if e.isalpha())
                    if(len(tag) == 0): tag = 'empty'
                    current_node.tag = tag
                #
                for attr in attrs:
                    try:
                        key, val = attr.split('=')
                        current_node.set(key, val[1:-1])
                    except ValueError:
                        try:
                            current_node.text.append(attr)
                        except AttributeError:
                            current_node.text = attr
            else:
                current_node = current_node.getparent()
                #
#    print(etree.tostring(root, pretty_print=True))
    return root
    

def convert_node(node):
    # tag
    data = ''
    data += '>{} '.format(node.tag)
    # attribs
    for attrib in node.attrib.iteritems():
        data += '{}="{}" '.format(attrib[0], attrib[1])
    # text if exists
    if(node.text != None):
        data += '{}'.format(node.text)
    # children
    for subnode in node:
        data += convert_node(subnode)
    # finish
    data += '<'
    data = data.replace('\n','')
    data = data.replace('\t','')
    data = data.strip()
    return data

def introduce_mutation(data):
    offset = random.randint(1, len(data))
    data2 = '{}{}{}'.format(data[:offset], '>', data[offset+1:])
    return data2

data2 = convert_node(tree)

print('parsing original')
node2 = convert_xml(data2)
f = open('sample_2.out', 'wb')
f.write(etree.tostring(node2, pretty_print=True))
f.close()

for i in range(0, 100):
    print('parsing mutation no {}'.format(i))
    data3 = introduce_mutation(data2)
    node3 = convert_xml(data3)
    f = open('sample_{}.out'.format(i), 'wb')
    f.write(etree.tostring(node3, pretty_print=True))
    f.close()

