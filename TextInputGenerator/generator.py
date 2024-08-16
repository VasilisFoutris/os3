import random
import string

def random_string(length):
    letters = string.ascii_letters + string.digits + string.punctuation
    return ''.join(random.choice(letters) for i in range(length))

with open('random_chars.txt', 'w') as file:
    for _ in range(1000):
        random_line = random_string(50)
        file.write(random_line + '\n')

print("Η διαδικασία ολοκληρώθηκε.")
