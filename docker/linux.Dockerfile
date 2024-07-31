FROM stateoftheartio/qt6:6.6-gcc-aqt

USER root

RUN apt-get update &&\
    apt-get -y install software-properties-common &&\
    add-apt-repository ppa:tomtomtom/yt-dlp &&\
    apt-get -y install libglx-dev libgl1-mesa-dev yt-dlp

WORKDIR /app

RUN mkdir /home/user/Downloads

RUN chown -R user /app /home/user/Downloads

COPY . /app

USER user

RUN mkdir -p /home/user/.cpm_cache
ENV CPM_SOURCE_CACHE=/home/user/.cpm_cache

RUN cmake -B build -S .\
    -G Ninja\
    -D CMAKE_BUILD_TYPE=Release\
    -D CMAKE_PREFIX_PATH=/opt/Qt/6.6.1/gcc_64

RUN cmake --build build --config Release --target yd_gui_tests

WORKDIR /app/build

CMD ctest --build-config Release --output-on-failure -E "yd_gui_qml_tests"
