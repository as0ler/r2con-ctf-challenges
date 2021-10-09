FROM debian:latest
RUN apt-get update
RUN apt-get install -y socat openssh-server 
RUN useradd -ms /home/ctf/game ctf
WORKDIR /home/ctf
ADD bin/ .
ADD config/sshd_config /etc/ssh/sshd_config
EXPOSE 1337
RUN echo "ctf:ctf" | chpasswd
RUN chown ctf:ctf /home/ctf/
RUN chown ctf:ctf /home/ctf/game
RUN chown ctf:ctf /home/ctf/flag.txt
RUN chmod 100 /home/ctf/game
RUN chmod 444 /home/ctf/flag.txt
RUN mkdir /run/sshd
RUN chmod 0755 /run/sshd
CMD ["/usr/sbin/sshd","-D"]
