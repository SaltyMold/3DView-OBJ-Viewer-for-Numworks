import struct

def parse_obj(filename):
    points = []
    edges = set()
    with open(filename) as f:
        for line in f:
            if line.startswith('v '):
                _, x, y, z = line.split()
                points.append((float(x), float(y), float(z)))
            elif line.startswith('f '):
                idx = [int(part.split('/')[0]) - 1 for part in line.split()[1:]]
                for i in range(len(idx)):
                    a, b = idx[i], idx[(i+1)%len(idx)]
                    edge = tuple(sorted((a, b)))
                    edges.add(edge)
    return points, list(edges)

def write_bin(points, edges, outname):
    with open(outname, 'wb') as f:
        f.write(struct.pack('<ii', len(points), len(edges)))
        for x, y, z in points:
            f.write(struct.pack('<fff', x, y, z))
        for a, b in edges:
            f.write(struct.pack('<ii', a, b))

points, edges = parse_obj('moto.obj')
write_bin(points, edges, 'monfichier.bin')