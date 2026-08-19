# Word frequency counter.
#
# Takes a mapping of filename -> file contents, counts the words in each
# file, reports the top words per file, and merges everything into a
# single total counter.
#
# Helpers come before their callers: the generated C++ is emitted in source
# order, and C++ needs a declaration before use.

from splice.stdlib import copy


def clean_word(raw: str) -> str:
    """Strip surrounding punctuation and normalise case."""
    return raw.strip(".,!?;:()").lower()


def words_in_line(line: str) -> list[str]:
    """Split a line into cleaned, non-empty words."""
    words: list[str] = []
    for raw in line.split():
        word = clean_word(raw)
        if word:
            words.append(word)
    return words


def count_words(contents: str) -> dict[str, int]:
    """Count every word in a file, line by line."""
    counts: dict[str, int] = {}
    for line in contents.splitlines():
        for word in words_in_line(line):
            counts[word] = counts.get(word, 0) + 1
    return counts


def rank(counts: dict[str, int]) -> list[tuple[int, str]]:
    """Order words by count, highest first.

    Pairs are (count, word) so the sort is total: ties break on the word,
    which keeps the result stable even though dict iteration order is not.
    """
    ranked: list[tuple[int, str]] = []
    for word in counts:
        ranked.append((counts[word], word))
    return sorted(ranked, reverse=True)


def top_n(counts: dict[str, int], n: int) -> list[tuple[int, str]]:
    """The n most frequent words, or all of them if there are fewer."""
    top: list[tuple[int, str]] = []
    taken = 0
    for entry in rank(counts):
        if taken < n:
            top.append(entry)
            taken = taken + 1
    return top


def count_all(files: dict[str, str]) -> dict[str, dict[str, int]]:
    """Count each file separately, keyed by filename."""
    per_file: dict[str, dict[str, int]] = {}
    for name in files:
        per_file[name] = count_words(files[name])
    return per_file


def merge_counts(per_file: dict[str, dict[str, int]]) -> dict[str, int]:
    """Fold every per-file counter into one total counter."""
    total: dict[str, int] = {}
    for name in per_file:
        counts = copy(per_file[name])
        for word in counts:
            total[word] = total.get(word, 0) + counts[word]
    return total


def total_count(counts: dict[str, int]) -> int:
    """How many words in total, counting repeats."""
    running = 0
    for word in counts:
        running = running + counts[word]
    return running


def words_shared_by_all(per_file: dict[str, dict[str, int]]) -> list[str]:
    """Words that appear in every file, as a sorted list."""
    shared: set[str] = set()
    first = True
    for name in per_file:
        current: set[str] = set()
        for word in per_file[name]:
            current.add(word)
        if first:
            shared = copy(current)
            first = False
        else:
            shared = shared & current
    return sorted(shared)


def report(name: str, counts: dict[str, int], n: int) -> None:
    """Print one file's summary."""
    print("---", name, "---")
    print("  words:", total_count(counts), "unique:", len(counts))
    for count, word in top_n(counts, n):
        print("   ", word, count)


def check(label: str, actual: int, expected: int) -> None:
    if actual == expected:
        print("ok  ", label)
    else:
        print("FAIL", label, "got", actual, "want", expected)


def check_str(label: str, actual: str, expected: str) -> None:
    if actual == expected:
        print("ok  ", label)
    else:
        print("FAIL", label, "got", actual, "want", expected)


def run_checks() -> None:
    """Exercise each helper on small, hand-checked inputs."""
    print("=== checks ===")
    check_str("clean_word strips punctuation", clean_word("Hello,"), "hello")
    check_str("clean_word lowercases", clean_word("WORLD"), "world")
    check_str("clean_word leaves bare words", clean_word("plain"), "plain")
    check("clean_word can empty a token", len(clean_word("...")), 0)

    check("words_in_line splits", len(words_in_line("a b c")), 3)
    check("words_in_line drops punctuation-only", len(words_in_line("a ... b")), 2)
    check("words_in_line on blank line", len(words_in_line("   ")), 0)

    sample = count_words("the cat\nthe hat")
    check("count_words totals repeats", sample["the"], 2)
    check("count_words counts singles", sample["cat"], 1)
    check("count_words unique words", len(sample), 3)
    check("count_words total", total_count(sample), 4)

    check("rank orders by count", rank(sample)[0][0], 2)
    check_str("rank puts the top word first", rank(sample)[0][1], "the")
    check("top_n caps the result", len(top_n(sample, 2)), 2)
    check("top_n allows fewer than n", len(top_n(sample, 99)), 3)

    merged = merge_counts(count_all({"a": "x y", "b": "x z"}))
    check("merge_counts sums across files", merged["x"], 2)
    check("merge_counts keeps unique words", len(merged), 3)
    print()


def main() -> int:
    files = {
        "intro.txt": "the quick brown fox\njumps over the lazy dog\nthe dog barks",
        "notes.txt": "the dog sleeps. the dog dreams!\na quick nap for the dog",
        "empty.txt": "",
    }

    run_checks()

    per_file = count_all(files)
    print("=== per file ===")
    for name in sorted(per_file):
        report(name, per_file[name], 3)
    print()

    total = merge_counts(per_file)
    print("=== totals ===")
    print("files:", len(per_file))
    print("words:", total_count(total), "unique:", len(total))
    for count, word in top_n(total, 5):
        print("   ", word, count)
    print()

    print("shared by every file:", words_shared_by_all(per_file))
    print("'dog' appears:", total["dog"], "times")
    print("'cat' present:", "cat" in total)
    return 0
