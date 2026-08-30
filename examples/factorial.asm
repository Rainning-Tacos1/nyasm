@code i386

@macro WRITE_SPACE()
    @byte 0x20

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

@macro WRITE_FACTORIAL(num)
    _n = 1
    @repeat num
        fact = 1
        i = 1
        @while i <= _n
            fact *= i
            i += 1

        WRITE_NUMBER(fact)
        WRITE_SPACE()
        @del fact
        @del i
        _n += 1
    @del _n

WRITE_FACTORIAL(21)