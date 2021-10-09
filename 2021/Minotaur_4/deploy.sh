#!/bin/bash
IMAGE=minotaur_4
docker build -t $IMAGE:latest .
docker stop $IMAGE && docker rm $IMAGE
docker run -d -p 4337:1337 --name $IMAGE $IMAGE

