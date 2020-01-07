FROM fedora:30

# assuming local directory is mounted at /project

RUN dnf install make -y
RUN mkdir /project
COPY INSTALL.mk /project/Makefile
RUN cd /project && make fedora && make SDL2 assimp
