with open("vehicles.csv", "w") as f:
    f.write("id,speed,temperature,fuel\n")
    for i in range(1, 2001):
        speed = 80 + (i % 41)  # 80-120
        temperature = 100 + (i % 31)  # 100-130
        fuel = 50 - (i % 40)   # 50-11
        f.write(f"{i},{speed},{temperature},{fuel}\n")