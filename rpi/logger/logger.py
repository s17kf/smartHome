import time


class Log:
    log_file = None
    stdout_min_lv = 0

    @staticmethod
    def log(level, log_msg):
        indent = level * '-'
        log_time = time.asctime(time.localtime(time.time()))
        if Log.log_file is not None and not Log.log_file.closed:
            written = Log.log_file.write('{}[{}] {}: {}\n'.format(indent, level, log_time, log_msg))
        if level <= Log.stdout_min_lv:
            print('{}[{}] {}: {}'.format(indent, level, log_time, log_msg))

    @staticmethod
    def init(filepath, std_output_min_lv = 0):
        Log.stdout_min_lv = std_output_min_lv;
        Log.log_file = open(filepath, 'w')
        if Log.log_file is None or Log.log_file.closed:
            Log.log(0, 'Failed to open log file {}'.format(filepath))
        else:
            Log.log(5, 'log file {} successfully opened'.format(filepath))

    @staticmethod
    def close():
        if Log.log_file is not None:
            Log.log_file.close()


if __name__ == '__main__':
    file_path = '../logs/file.log'
    # print(log_file is None)
    # log_file = open(file_path, 'w')
    try:
        Log.init(file_path, 5)
    except FileNotFoundError as error:
        Log.log(0, 'Failed to open logfile: {}'.format(error))

    Log.log(1, 'some log')
    Log.log(3, 'some other log')

    Log.close()

