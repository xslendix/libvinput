#!/bin/bash
# From: https://github.com/pypa/python-manylinux-demo/blob/master/travis/build-wheels.sh
# which is in the public domain.
#
# This is run inside a CentOS 5 virtual machine to build manylinux wheels:
#
#   $ docker run -v `pwd`:/io quay.io/pypa/manylinux1_x86_64 /io/ci/build_manylinux.sh
#

set -e -x

action=$1

yum --assumeyes install epel-release
yum --assumeyes update
yum --assumeyes install libxdo-devel libX11-devel libXtst-devel libXext-devel

if [[ $action == "build" ]]; then
    # Compile wheels
    cd /io
    for PYBIN in /opt/python/*/bin; do
				if echo "$PYBIN" | grep 3; then
					"$PYBIN/python" setup.py bdist_wheel -d ~/wheelhouse/
				fi
    done
    cd ~

    # Bundle external shared libraries into the wheels
    for whl in wheelhouse/*.whl; do
				export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/io
        auditwheel repair "$whl" -w /io/dist/
    done

elif [[ $action == "test" ]]; then
    # Install packages and test
    TOXBIN=/opt/python/cp27-cp27m/bin
    "$TOXBIN/pip" install -r /io/requirements/ci.pip

    for PYBIN in /opt/python/*/bin/; do
        PYNAME=$("$PYBIN/python" -c "import sys; print('python{0[0]}.{0[1]}'.format(sys.version_info))")
        TOXENV=$("$PYBIN/python" -c "import sys; print('py{0[0]}{0[1]}'.format(sys.version_info))")
        ln -s "$PYBIN/$PYNAME" /usr/local/bin/$PYNAME
        "$TOXBIN/tox" -e $TOXENV
        rm -f /usr/local/bin/$PYNAME
        #"${PYBIN}/pip" install python-manylinux-demo --no-index -f /io/dist
        #(cd "$HOME"; "${PYBIN}/nosetests" pymanylinuxdemo)
    done

else
    echo "Need an action to perform!"
fi
