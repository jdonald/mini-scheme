(print (+ 1 2 3))

(print (* 4 5))

(print (- 10 3))

(print (/ 20 4))

(print (% 17 5))

(print (= 5 5))

(print (< 3 5))

(print (> 5 3))

(if (< 3 5) (print 100) (print 200))

(and (< 1 2) (> 5 3))

(or 0 (< 3 5))

(define (square x)
  (* x x))

(print (square 7))

(define (factorial n)
  (if (= n 0)
    1
    (* n (factorial (- n 1)))))

(print (factorial 5))
