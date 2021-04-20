from runner import runner

import os, pytest

@pytest.fixture(scope="function")
def change_test_dir(request):
    os.chdir(request.fspath.dirname)
    yield
    os.chdir(request.config.invocation_dir)

def test_load_can_load_tests_from_dig(change_test_dir):
    tests = runner.load('./data/test.dig')

    expected = [{'name': 'Testcase 00', 'test': 'A B Out\n0 0 0\n'},
                {'name': 'Testcase 11', 'test': 'A B Out\n1 1 1\n'}]

    sorter = lambda lst: sorted(lst, key = lambda k: k['name'])

    assert sorter(tests) == sorter(expected)

def test_load_can_load_tests_from_tst(change_test_dir):
    tests = runner.load('./data/test.tst')

    assert tests == [{'name': './data/test.tst', 'test': 'A B Out\n0 0 0\n0 1 1\n1 0 1\n1 1 1\n'}]


def test_load_raises_exception_if_file_not_found(change_test_dir):
    with pytest.raises(FileNotFoundError):
        runner.load('./data/does-not-exist.dig')
