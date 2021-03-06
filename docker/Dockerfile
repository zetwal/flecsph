FROM laristra/flecsi-third-party:fedora

ARG MPI
ARG COVERAGE
ARG SONARQUBE
ARG SONARQUBE_TOKEN
ARG SONARQUBE_GITHUB_TOKEN
ARG CC
ARG CXX
ARG CXXFLAGS

#for coverage
ARG CI
ARG TRAVIS
ARG TRAVIS_BRANCH
ARG TRAVIS_JOB_NUMBER
ARG TRAVIS_PULL_REQUEST 
ARG TRAVIS_JOB_ID
ARG TRAVIS_TAG
ARG TRAVIS_REPO_SLUG
ARG TRAVIS_COMMIT
ARG TRAVIS_OS_NAME

RUN rm -rf /home/flecsi/.ccache
COPY flecsph /home/flecsi/flecsph
COPY ccache/ /home/flecsi/.ccache
COPY sonar/ /home/flecsi/.sonar
USER root
RUN chown -R flecsi:flecsi /home/flecsi/flecsph /home/flecsi/.ccache /home/flecsi/.sonar
USER flecsi

#build flecsi
WORKDIR /home/flecsi
RUN git clone -b FleCSPH --depth 1 --recursive https://github.com/laristra/flecsi flecsi && \
    cd flecsi && mkdir build && cd build && \
    cmake -DFLECSI_RUNTIME_MODEL=legion .. && \
    make -j2
WORKDIR flecsi/build
USER root
RUN make install
USER flecsi

# build H5hut
WORKDIR /home/flecsi
ENV H5Hut H5hut-1.99.13
RUN wget --no-check-certificate https://amas.psi.ch/H5hut/raw-attachment/wiki/DownloadSources/${H5Hut}.tar.gz
RUN tar -xzf ${H5Hut}.tar.gz
RUN wget -O- https://raw.githubusercontent.com/gentoo/gentoo/e8827415e767b1252f3851edef8e000980b7f2a1/sci-libs/h5hut/files/h5hut-1.99.13-autotools.patch | patch -p0
WORKDIR ${H5Hut}
RUN sed -i 's/H5Pset_fapl_mpiposix/H5Pset_fapl_mpio/g' src/h5core/h5_hdf5_private.h
RUN autoreconf -i -f
#ubuntu needs CC=h5pcc amd HDF5_USE_SHLIB=yes is needed for shared lib
RUN ./configure --disable-static --enable-shared --enable-parallel --prefix=/usr/local CC=h5pcc && HDF5_USE_SHLIB=yes make
USER root
RUN make install
USER flecsi
RUN ldd /usr/local/lib/libH5hut.so

WORKDIR /home/flecsi/flecsph
RUN mkdir build

WORKDIR build
RUN ccache -z
RUN cmake -DENABLE_MPI=$MPI \
          -DENABLE_UNIT_TESTS=ON \
          -DENABLE_OPENMP=ON \
          -DENABLE_LEGION=ON \
          -DENABLE_DOXYGEN=ON \
          ${COVERAGE:+-DENABLE_COVERAGE_BUILD=ON} ..

RUN HDF5_USE_SHLIB=yes ${SONARQUBE:+build-wrapper-linux-x86-64 --out-dir bw-output} make VERBOSE=1 -j2
RUN ccache -s
RUN make doxygen
RUN if [ ${COVERAGE} ]; then \
      python -m coverxygen --xml-dir doc/doxygen/xml/ --src-dir .. --output doxygen.coverage.info; \
      wget -O codecov.sh https://codecov.io/bash; \
      bash codecov.sh -X gcov -f doxygen.coverage.info -F documentation; \
#      doxy-coverage --threshold 64 doc/doxygen/xml/; \
    fi
RUN make install DESTDIR=${PWD}/install && rm -rf ${PWD}/install
RUN make test ARGS="-V" || true
RUN cd .. && if [ ${COVERAGE} ]; then \
  if [ ${CC} = clang ]; then \
    $HOME/.local/bin/codecov -F "${CC}" --gcov-exec "llvm-cov gcov"; \
  else \
    $HOME/.local/bin/codecov -F "${CC}"; \
  fi; \
fi && cd -
RUN cd .. && if [ ${SONARQUBE} ]; then \
  sonar-scanner \
    -Dsonar.projectKey=${TRAVIS_REPO_SLUG##*/} \
    -Dsonar.projectName=${TRAVIS_REPO_SLUG#*/} \
    -Dsonar.projectVersion=${TRAVIS_COMMIT} \
    -Dsonar.branch=/${TRAVIS_BRANCH} \
    -Dsonar.links.homepage=http://${TRAVIS_REPO_SLUG%%/*}.github.io/${TRAVIS_REPO_SLUG#*/} \
    -Dsonar.links.ci=https://travis-ci.org/${TRAVIS_REPO_SLUG} \
    -Dsonar.links.scm=https://github.com/${TRAVIS_REPO_SLUG} \
    -Dsonar.links.issue=https://github.com/${TRAVIS_REPO_SLUG}/issues \
    -Dsonar.sources=. \
    -Dsonar.exclusions=build/CMakeFiles/**,cinch/**,build/doc/doxygen/**,build/include/** \
    -Dsonar.cfamily.build-wrapper-output=build/bw-output \
    -Dsonar.cfamily.gcov.reportsPath=. \
    -Dsonar.host.url=https://sonarqube.com \
    -Dsonar.organization=${TRAVIS_REPO_SLUG%%/*} \
    $([ ${TRAVIS_PULL_REQUEST} != false ] && echo \
       -Dsonar.analysis.mode=preview \
       -Dsonar.github.pullRequest=${TRAVIS_PULL_REQUEST} \
       -Dsonar.github.repository=${TRAVIS_REPO_SLUG} \
       -Dsonar.github.oauth=${SONARQUBE_GITHUB_TOKEN}) \
    -Dsonar.login=${SONARQUBE_TOKEN}; \
fi && cd -
USER root
RUN make install
USER flecsi
WORKDIR /home/flecsi
