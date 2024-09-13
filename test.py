def encodedDistance(inches):
    start = 65535
    vector = [0, 1, 0, 1, 1, 0, 1, 0, 1, 1,
              0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1]
    # we always drop 39935
    # we always rise 25600
    result = []
    stage = 0

    for i in range(inches + 1):
        if (vector[stage] == 0):
            result.append(start - 39935)
            start = start - 39935
        else:
            result.append(start + 25600)
            start = start + 25600

        stage = stage + 1

        if (stage == 23):
            stage = 0

    return int(hex(result[i-1]), 16)

width = 24
height = 30
newWidth = min(44, width)
height = int(height * (newWidth / width))
width = newWidth

print("height:", hex(encodedDistance(height)))
print("width:", hex(encodedDistance(width)))