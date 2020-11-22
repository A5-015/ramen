#!/usr/bin/python3

import json
import base64
import sys

if len(sys.argv) > 1:
    print(base64.urlsafe_b64decode(sys.argv[1].encode()).decode())
