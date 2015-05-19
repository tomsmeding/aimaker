#!/bin/sh

find . -type f -name "*.cpp" | xargs astyle --style=google --indent=force-tab=4 --indent-namespaces --pad-header --pad-oper --keep-one-line-statements --align-pointer=name --align-reference=name --suffix=none
