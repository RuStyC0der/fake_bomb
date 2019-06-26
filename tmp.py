access_time = 51040
minuts = access_time / (1000 * 60)
seconds = ((access_time % (1000 * 60)) / 1000)
print(minuts //10)
print(minuts % 10)
print(seconds // 10 )
print(seconds % 10)
