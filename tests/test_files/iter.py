# from typing import Iterable
#
#
# def process_enumerated(enumerated: Iterable) -> int:
#     """Function that accepts an iterable and processes it."""
#     count = 0
#     for i, val in enumerated:
#         print(i, val)
#         count = count + 1
#     return count
#
#
# def process_zipped(zipped: Iterable) -> int:
#     """Function that accepts a zipped iterable."""
#     count = 0
#     for x, y in zipped:
#         print(x, y)
#         count = count + 1
#     return count
#
#
# def process_mapped(mapped: Iterable) -> int:
#     """Function that accepts a mapped iterable."""
#     total = 0
#     for val in mapped:
#         print(val)
#         total = total + val
#     return total
#
#
# def process_filtered(filtered: Iterable) -> int:
#     """Function that accepts a filtered iterable."""
#     count = 0
#     for val in filtered:
#         print(val)
#         count = count + 1
#     return count
#
#
# def main() -> int:
#     print("=== Test 1: Assign and iterate enumerate ===")
#     nums = [10, 20, 30]
#     e = enumerate(nums)
#     for i, n in e:
#         print("enumerate:", i, n)
#
#     print("\n=== Test 2: Pass enumerate to function ===")
#     result = process_enumerated(enumerate([100, 200, 300]))
#     print("Processed:", result, "items")
#
#     print("\n=== Test 3: Assign and iterate zip ===")
#     a = [1, 2, 3]
#     b = ["x", "y", "z"]
#     z = zip(a, b)
#     for x, y in z:
#         print("zip:", x, y)
#
#     print("\n=== Test 4: Pass zip to function ===")
#     result2 = process_zipped(zip([10, 20], ["a", "b"]))
#     print("Processed:", result2, "pairs")
#
#     print("\n=== Test 5: Assign and iterate map ===")
#     nums2 = [1, 2, 3, 4]
#     m = map(lambda x: x * 2, nums2)
#     for doubled in m:
#         print("mapped:", doubled)
#
#     print("\n=== Test 6: Pass map to function ===")
#     result3 = process_mapped(map(lambda x: x * 3, [1, 2, 3]))
#     print("Sum of mapped:", result3)
#
#     print("\n=== Test 7: Assign and iterate filter ===")
#     nums3 = [1, 2, 3, 4, 5, 6]
#     f = filter(lambda x: x % 2 == 0, nums3)
#     for even in f:
#         print("filtered:", even)
#
#     print("\n=== Test 8: Pass filter to function ===")
#     result4 = process_filtered(filter(lambda x: x > 2, [1, 2, 3, 4, 5]))
#     print("Filtered:", result4, "items")
#
#     print("\n=== Test 9: Nested - enumerate(zip()) ===")
#     list_a = [10, 20, 30]
#     list_b = ["a", "b", "c"]
#     nested = enumerate(zip(list_a, list_b))
#     for idx, pair in nested:
#         i, v = pair
#         print("nested:", idx, i, v)
#
#     print("\n=== Test 10: Filter(map()) in for loop ===")
#     nums4 = [1, 2, 3, 4, 5]
#     filtered_mapped = filter(lambda x: x > 5, map(lambda x: x * x, nums4))
#     for val in filtered_mapped:
#         print("filter(map):", val)
#
#     print("\n=== Test 11: Multiple assignment and reuse ===")
#     data1 = [100, 200, 300]
#     data2 = ["x", "y", "z"]
#     z1 = zip(data1, data2)
#     e1 = enumerate(z1)
#     for idx, pair in e1:
#         a, b = pair
#         print("multi:", idx, a, b)
#
#     print("\n=== Test 12: Enumerate(filter(map())) ===")
#     nums5 = [1, 2, 3, 4, 5, 6]
#     complex_iter = enumerate(filter(lambda x: x > 5, map(lambda x: x * 2, nums5)))
#     count = 0
#     for idx, val in complex_iter:
#         print("complex:", idx, val)
#         count = count + 1
#     print("Complex iteration count:", count)
#
#     return 0


from splice.stdlib import copy


def main() -> int:
    nums = [1, 2, 3, 4, 5]
    a = list(map(lambda x: x * 2, nums))
    print(a)
    for s in map(lambda x: str(x), nums):
        print(s)
    filtered = list(filter(lambda x: x % 2 == 0, nums))
    print(filtered)
    for x, y in zip(nums, a):
        print(x, y)
    nums = copy(a)
    for i, n in enumerate(nums):
        print(i, n)
    return 0
