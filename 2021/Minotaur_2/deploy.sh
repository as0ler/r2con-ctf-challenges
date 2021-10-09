#!/bin/bash
IMAGE=minotaur_2
docker build -t $IMAGE:latest .
docker stop $IMAGE && docker rm $IMAGE
docker run -d -p 2337:1337 --name $IMAGE $IMAGE

