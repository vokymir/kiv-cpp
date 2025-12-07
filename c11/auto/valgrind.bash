#!/bin/bash

valgrind --leak-check=full -s --show-leak-kinds=all ./cpp
