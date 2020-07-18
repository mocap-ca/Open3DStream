import sys

import MyGame.Sample as proto
from MyGame.Sample.SubjectList import SubjectList

import flatbuffers

import socket
import struct

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind(("", 5566))

    s.listen()

    conn, addr = s.accept()

    buffer = b''

    with conn:
        print('Connected by', addr)
        while True:
            data = conn.recv(1024)
            if not data:
                break
            buffer += data

            if len(buffer) > 4:
                if buffer[0] != 0x03 or buffer[1] != 0x02:
                    print("Invalid stream")
                    print(hex(buffer[0]))
                    print(hex(buffer[1]))
                    print(hex(buffer[2]))
                    print(hex(buffer[3]))
                    break

                bucket_len, = struct.unpack("i", buffer[4:8])

                if len(buffer) >= (bucket_len + 8):

                    items = SubjectList.GetRootAsSubjectList(buffer[8:bucket_len+8], 0)

                    for i in range(items.SubjectsLength()):
                        subject = items.Subjects(i)
                        print(subject.Name())
                        print(subject.NodesLength())
                        print(subject.NamesLength())

            print(len(data))

