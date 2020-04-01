import argparse
from graphviz import Source

parser = argparse.ArgumentParser(description='Merge connection graph and forest data.')
parser.add_argument('dot_file', type=str, help='dot file')
args = parser.parse_args()

path = args.dot_file
s = Source.from_file(path)
s.view()
