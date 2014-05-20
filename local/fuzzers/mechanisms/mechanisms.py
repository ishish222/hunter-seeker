import other_parts

class step(object):
    def __init__(self,
            operation = other_parts.nop, 
            is_final = False, 
            test = other_parts.fail_check, 
            should_retry = False,
            retries = 0,
            failure = "exit_failure",
            next_step = None
            ):
        self.operation = operation
        self.is_final = is_final
        self.test = test
        self.should_retry = should_retry
        self.retries = retries
        self.failure = failure
        self.next_step = next_step
