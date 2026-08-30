@code i386

@macro WRITE_NUMBER(num)
    n = num
    digits = []
    count = 0
    @while n > 0
        digits[] = (n % 10)
        count += 1
        n /= 10
    count -= 1

    @while count >= 0
        @byte 48 + digits[count]
        count -= 1

    @del digits
    @del n
    @del count


MAX = 10

# explicitly create arrays
prev = []
current = []

row = 0

# initialize prev with zeros
col = 0
@while col < MAX
    prev[] = 0
    current[] = 0
    col = col + 1

@while row < MAX

    # reset current row
    col = 0
    @while col < MAX
        current[col] = 0
        col = col + 1

    # build row
    col = 0
    @while col <= row

        @if col == 0
            value = 1
        @else
            @if col == row
                value = 1
            @else
                value = prev[col - 1] + prev[col]

        current[col] = value

        WRITE_NUMBER(value)
        @string " "

        col = col + 1

    @string "\n"

    // Ola
    # copy current -> prev
    col = 0
    @while col <= row
        prev[col] = current[col]
        col = col + 1

    row = row + 1