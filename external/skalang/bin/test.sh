#!/usr/bin/env bash

BASEDIR=$(dirname "$0")
echo "In '$BASEDIR'"

$BASEDIR/skalang_test 1> /dev/null && \
$BASEDIR/skalang_bytecode_generator_test 1> /dev/null && \
$BASEDIR/skalang_bytecode_interpreter_test 1> /dev/null && \
echo "SUCCESS" || echo "FAILURE"
