import glob

for filename in glob.glob("*.cpp") + glob.glob("*.h"):
    with open(filename, "rb") as f:
        data = f.read()

    if b"\r\n" in data:
        data = data.replace(b"\r\n", b"\n")
        with open(filename, "wb") as f:
            f.write(data)
        print(f"converted: {filename}")
    else:
        print(f"already LF: {filename}")
