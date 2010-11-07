import time
import threading
import json
from urllib import urlopen, pathname2url, urlencode

def run_in_thread(fn):
    def run(*k, **kw):
        t = threading.Thread(target=fn, args=k, kwargs=kw)
        t.start()
    return run

SERVER = 'http://127.0.0.1:%s/%s'

READ_PORT = 8080
WRITE_PORT = 9080

NO_OFFSET = 0
WAITING_DATA = -1
NEEDS_RESYNC = -3
TEMP_DATA = None


def test_no_data_read():
    reset_channel('test')

    response = read_channel('test', NO_OFFSET)
    assert response['new_offset'] == -1


def test_write_data():
    response = write_channel('test', 'hello world')

    response = read_channel('test', NO_OFFSET)
    assert response['new_offset'] == 1

    response = read_channel('test', WAITING_DATA)
    assert response['new_offset'] == 1
    assert len(response['data']) == 1


def test_error_offset():
    reset_channel('test')

    response = read_channel('test', 4)
    assert response['new_offset'] == WAITING_DATA

    response = write_channel('test', 'hello world')

    response = read_channel('test', 3)
    assert response['new_offset'] == NEEDS_RESYNC

    response = read_channel('test', 0)
    assert response['new_offset'] == 1


def test_too_many_message():
    reset_channel('test')

    for j in range(1, 101):
        write_channel('test', 'hello world %s' % j)

    #--- Check that we aren't adding too many messages
    response = read_channel('test', WAITING_DATA, timeout=0)
    assert response['new_offset'] > 1
    assert len(response['data']) == 20
    print response['data']
    assert response['data'][-1]['msg'] == 'hello world 100'

    #--- Check that selecting by offset works
    response = read_channel('test', response['new_offset']-10, timeout=0)
    assert len(response['data']) == 10
    assert response['data'][0]['msg'] == 'hello world 91'

    #--- Check that selecting by offset works
    response = read_channel('test', response['new_offset']-1, timeout=0)
    assert len(response['data']) == 1
    assert response['data'][0]['msg'] == 'hello world 100'

    #--- Selecting by an unknown offset should force us to resync
    response = read_channel('test', 5, timeout=0)
    assert len(response['data']) == 0
    assert response['new_offset'] == NEEDS_RESYNC



def test_waiting_on_message_1():
    global TEMP_DATA
    TEMP_DATA = None

    reset_channel('test_waiting')

    @run_in_thread
    def init_read():
        global TEMP_DATA
        response = read_channel('test_waiting', NO_OFFSET, timeout=1000)
        TEMP_DATA = response
    init_read()

    write_channel('test_waiting', 'hello world')

    time.sleep(0.5)

    assert TEMP_DATA != None
    assert TEMP_DATA['new_offset'] == 1
    assert len(TEMP_DATA['data']) == 1
    assert TEMP_DATA['data'][0]['msg'] == 'hello world'

def test_waiting_on_message_2():
    global TEMP_DATA
    TEMP_DATA = None

    reset_channel('test_waiting')

    @run_in_thread
    def init_read():
        global TEMP_DATA
        response = read_channel('test_waiting', WAITING_DATA, timeout=1000)
        TEMP_DATA = response
    init_read()

    write_channel('test_waiting', 'hello world')

    time.sleep(0.5)

    assert TEMP_DATA != None
    assert TEMP_DATA['new_offset'] == 1
    assert len(TEMP_DATA['data']) == 1
    assert TEMP_DATA['data'][0]['msg'] == 'hello world'

def test_is_active():
    close_channel('test_active')
    assert is_active('test_active') == False

    write_channel('test_active', 'hello world', force=True)
    assert is_active('test_active') == True

    close_channel('test_active')
    assert is_active('test_active') == False

    read_channel('test_active', 0)
    assert is_active('test_active') == True

    write_channel('test_active,test_active_2', 'hello world', force=True)
    assert is_active('test_active_2') == True

def test_sids():
    close_channel('test_sids_1')
    close_channel('test_sids_2')

    for i in range(0, 50):
        write_channel('test_sids_1,test_sids_2', 'hello world %s' % i, force=True)

    assert is_active('test_sids_1') == True
    assert is_active('test_sids_2') == True

    response_1 = read_channel('test_sids_1', WAITING_DATA, timeout=0)
    response_2 = read_channel('test_sids_2', WAITING_DATA, timeout=0)

    assert len(response_1['data']) > 0
    assert len(response_2['data']) > 0

    for d1 in response_1['data']:
        for d2 in response_2['data']:
            if d1['msg'] == d2['msg']:
                assert d1['_cid'] == d2['_cid']

    response_3 = read_channel('test_sids_2', 0, timeout=0)
    assert response_3['new_offset'] > 0


#--- Helpers ----------------------------------------------
def send_req(method, args={}, port=READ_PORT):
    url = SERVER % (port, method)
    r_url = '%s?%s' % (url, urlencode(args))
    data = urlopen(r_url).read()
    #print data
    return json.loads(data)

def write_channel(name, message, force=False):
    #print '#', 'write_channel(%s, %s, %s)' % (name, message, force)
    data = {
        'channels': name,
        'data': json.dumps({'msg': message})
    }
    if force:
        data['force'] = 1
    response = send_req('comet', data , port=WRITE_PORT)
    try:
        assert response['status'] == 'ok'
    except:
        print response
        raise
    return response

def is_active(name):
    data = {
        'channels': name,
        'type': 'is_active',
        'data': None
    }

    response = send_req('comet', data, port=WRITE_PORT)
    print response
    if name in response:
        return True
    else:
        return False

def reset_channel(name):
    #print '#', 'reset_channel(%s)' % name
    response = send_req('comet', {
        'channels': name,
        'type': 'reset',
        'data': None
    }, port=WRITE_PORT)
    assert response['status'] == 'ok'
    return response

def close_channel(name):
    #print '#', 'close_channel(%s)' % name
    response = send_req('comet', {
        'channels': name,
        'type': 'close',
        'data': None
    }, port=WRITE_PORT)
    assert response['status'] == 'ok'
    return response

def read_channel(name, offset, timeout=50):
    #print '#', 'read_channel(%s, %s, %s)' % (name, offset, timeout)
    response = send_req('comet',
            { 'channel': name,
              'offset': offset,
              'timeout': timeout })
    return response
