#!/bin/bash
IMAGE=minotaur_1
docker build -t $IMAGE:latest .
docker stop $IMAGE && docker rm $IMAGE
docker run -d -p 1337:1337 --name $IMAGE $IMAGE

