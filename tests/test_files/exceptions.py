def guarded_parse(text: str) -> int:
    """finally must run when the try returns, and when it raises through."""
    try:
        return int(text)
    finally:
        print("cleanup", text)


def parse_or(text: str, fallback: int) -> int:
    """else runs only when nothing was raised."""
    try:
        value = int(text)
    except ValueError:
        print("bad literal", text)
        return fallback
    else:
        print("good literal", text)
        return value


def nested() -> int:
    try:
        try:
            return int("nope")
        finally:
            print("inner finally")
    except ValueError:
        print("outer caught")
        return -1
    finally:
        print("outer finally")


def handler_raises() -> int:
    """A raising handler skips else and still runs finally on the way out."""
    try:
        try:
            print(int("bad"))
        except ValueError:
            print("handler raising")
            return int("worse")
        else:
            print("not reached")
        finally:
            print("guard ran")
    except ValueError:
        print("caught the handler's exception")
    return 0


def check_positive(n: int) -> int:
    if n < 0:
        raise ValueError("negative")
    return n


def reraise(n: int) -> int:
    """A bare raise passes the live exception on, finally still runs."""
    try:
        return check_positive(n)
    except ValueError:
        print("logging and passing it on")
        raise
    finally:
        print("reraise finally")


def raise_bare_class(key: str) -> int:
    raise KeyError


def siblings(first: str, second: str) -> int:
    """Two try statements in one scope must not collide over their flags."""
    try:
        print(int(first))
    except ValueError:
        print("first was bad")
    else:
        print("first was fine")
    try:
        print(int(second))
    except ValueError:
        print("second was bad")
    else:
        print("second was fine")
    return 0


def relay() -> int:
    """`raise e` keeps the real type, rather than the one the handler declared."""
    try:
        raise ValueError("original")
    except Exception as e:
        print("relaying")
        raise e


def main() -> int:
    print(guarded_parse("41"))
    try:
        print(guarded_parse("zzz"))
    except ValueError:
        print("caught from callee")

    print(parse_or("7", 0))
    print(parse_or("seven", 0))
    print(nested())
    print(handler_raises())

    print(check_positive(3))
    try:
        print(check_positive(-1))
    except ValueError:
        print("caught the raise")

    try:
        print(reraise(-2))
    except ValueError:
        print("caught the re-raise")

    try:
        print(raise_bare_class("k"))
    except KeyError:
        print("caught the bare class")

    print(siblings("8", "eight"))

    try:
        print(relay())
    except ValueError:
        print("still a ValueError after the relay")

    # a raised subclass is still caught by the base handler
    try:
        raise TypeError("wrong type")
    except Exception:
        print("base handler took the subclass")

    numbers = [1, 2, 3]
    try:
        print(numbers[10])
    except IndexError:
        print("index error wins over the base class")
    except Exception:
        print("not reached")

    items = [1, 2, 3]
    try:
        print(items[10])
    except:
        print("bare except caught it")

    try:
        print(float("x"))
    except ValueError as e:
        print("float refused it")

    # finally has to run on the way out of the loop too
    for i in range(3):
        try:
            if i == 1:
                break
            print("loop", i)
        finally:
            print("loop finally", i)

    return 0
