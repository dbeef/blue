FROM fedora:30

# assuming local directory is mounted at /project

RUN dnf install make -y
RUN cd /project && make fedora && make SDL2 assimp
