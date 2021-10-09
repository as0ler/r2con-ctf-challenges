#!/bin/bash
IMAGE=minotaur_extra
docker build -t $IMAGE:latest .
docker stop $IMAGE && docker rm $IMAGE
docker run -d -p 5337:1337 --name $IMAGE $IMAGE
