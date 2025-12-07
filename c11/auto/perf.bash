#!/bin/bash

sudo perf record ./cpp
sudo perf report
