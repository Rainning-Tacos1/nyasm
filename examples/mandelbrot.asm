@code i386

maxIter = 50

y = -1.5
@while y <= 1.5

    x = -2.0
    @while x <= 1.0

        a = 0
        b = 0
        iter = 0

        @while iter < maxIter

            aa = a * a
            bb = b * b

            @if (aa + bb) > 4
                @break

            temp = aa - bb + x
            b = 2 * a * b + y
            a = temp

            iter = iter + 1

        @if iter == maxIter
            @string "."
        @else
            @string "█"

        x = x + 0.05

    @string "\n"
    y = y + 0.05