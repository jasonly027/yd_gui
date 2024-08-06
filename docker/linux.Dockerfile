FROM stateoftheartio/qt6:6.6-gcc-aqt

USER root

RUN apt-get update &&\
    apt-get -y install libglx-dev libgl1-mesa-dev

# Setup yt-dlp
RUN curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -o /usr/local/bin/yt-dlp &&\
    chmod a+rx /usr/local/bin/yt-dlp

# Setup CPM.cmake cache
RUN mkdir -p /home/user/.cpm_cache &&\
    chown -R user /home/user/.cpm_cache
ENV CPM_SOURCE_CACHE=/home/user/.cpm_cache

# Create app dir
RUN mkdir -p /app &&\
    chown -R user /app

COPY . /app

WORKDIR /app

USER user

# Build & Test

RUN cmake -B build -S .\
    -G Ninja\
    -D CMAKE_BUILD_TYPE=Release\
    -D CMAKE_PREFIX_PATH=/opt/Qt/6.6.1/gcc_64

RUN cmake --build build --config Release --target yd_gui_tests

WORKDIR /app/build

CMD ctest --build-config Release --output-on-failure -E "yd_gui_qml_tests"
