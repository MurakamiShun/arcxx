FROM gcc:11.2
RUN apt-get update && apt-get -y install cmake make git libsqlite3-dev libpq-dev
WORKDIR /workspace
CMD /bin/bash