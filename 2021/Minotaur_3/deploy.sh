#!/bin/bash
IMAGE=minotaur_3
docker build -t $IMAGE:latest .
docker stop $IMAGE && docker rm $IMAGE
docker run -d -p 3337:1337 --name $IMAGE $IMAGE

