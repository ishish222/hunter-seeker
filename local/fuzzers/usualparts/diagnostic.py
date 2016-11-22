import time
import globs

class statistics(object):
    def __init__(self, metrics):
        self.metric_res = metrics
        self.sample_count = 0
        self.to_count = 0
        self.ma_count = 0
        self.last_time_check = time.localtime()

def prepare_stats(args=None):
    options = globs.state.options
    globs.state.stats = statistics(options.metric_res)
