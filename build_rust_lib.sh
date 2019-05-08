#!/bin/sh
cd something
cargo build
cp target/debug/libsomething.so ../libsomething.so
