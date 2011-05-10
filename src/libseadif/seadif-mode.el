;; Emacs SEADIF mode for sea-of-gates data base editing.   Seadif is the
;; Edif-like language used by the OCEAN sea-of-gates tools of Delft
;; University of Technology, the Netherlands.
;;
;; This file is not part of GNU emacs, but the usual GNU copyrights apply.
;;
;; July 1990. Direct comments and suggestions to stravers@donau.et.tudelft.nl
;;
;; @(#)seadif-mode.el 1.1 07/28/90 
;;
;; Just put (load "seadif-mode") in your .emacs and you'll be fine.
;;

;; Seadif files end in ".sdf" or --sometimes-- in ".seadif"
(setq auto-mode-alist (cons '("\\.\\(sdf\\|seadif\\)$" . sdf-mode)
			    auto-mode-alist))

(defvar sdf-mode-abbrev-table nil
	"Abbrev table in use in sdf-mode buffers.")
(define-abbrev-table 'sdf-mode-abbrev-table ())

(defvar sdf-mode-map () "Keymap used in SEADIF mode.")
(if sdf-mode-map
    ()
    (setq sdf-mode-map (make-sparse-keymap))
    (define-key sdf-mode-map "\C-j" 'sdf-newline-and-indent)
    (define-key sdf-mode-map "\C-i" 'sdf-indent-line)
    (define-key sdf-mode-map "\M-\C-h" 'sdf-mark-sexp)
    (define-key sdf-mode-map "\C-c." 'sdf-where-am-i)
    (define-key sdf-mode-map "\C-x\C-u" 'sdf-undefined)	; too dangerous...
    (define-key sdf-mode-map "\C-x\C-l" 'sdf-undefined)	; also too dangerous
    )

(defvar sdf-mode-syntax-table nil
	"Syntax table in use in sdf-mode buffers.")

(defvar sdf-indent-amount 3
	"Number of spaces to insert per indent level in a Seadif expression.")

(defvar sdf-align-paren t
	"If non-nil align closing parenthesis with opening parenthesis")

(defvar sdf-default-comment-column 48
	"Column where /* ... */ comments go.")

(if sdf-mode-syntax-table
    ()
  (setq sdf-mode-syntax-table (make-syntax-table))
  (modify-syntax-entry ?/ "_ 14" sdf-mode-syntax-table)	; c-like comment delimiters
  (modify-syntax-entry ?* "w 23" sdf-mode-syntax-table)	; c-like comment delimiters
  (modify-syntax-entry ?\" "\"" sdf-mode-syntax-table)  ; double quote delimits string
  (modify-syntax-entry ?% "w" sdf-mode-syntax-table) ; percent belongs to word
  (modify-syntax-entry ?! "_" sdf-mode-syntax-table)
  (modify-syntax-entry ?# "_" sdf-mode-syntax-table)
  (modify-syntax-entry ?_ "_" sdf-mode-syntax-table)
  (modify-syntax-entry ?. "_" sdf-mode-syntax-table)
  (modify-syntax-entry ?[ "w" sdf-mode-syntax-table) ; nothing special with '['
  (modify-syntax-entry ?] "w" sdf-mode-syntax-table) ; nothing special with ']'
  (modify-syntax-entry ?{ "w" sdf-mode-syntax-table) ; nothing special with '{'
  (modify-syntax-entry ?} "w" sdf-mode-syntax-table) ; nothing special with '}'
  (modify-syntax-entry ?( "()" sdf-mode-syntax-table) ; '(' only matches ')'
  (modify-syntax-entry ?) ")(" sdf-mode-syntax-table)) ; ')' only matches '('

(defun sdf-mode ()
  "Major mode for editing SEADIF expressions.
Comments are delimited with /* ... */. Multi-line comments are recognized and
indented properly. TAB indents the current line.  Hitting NEWLINE (\\C-j) in
stead of RETURN causes the current line and the new line to be indented.

If you're editing a multi-line comment NEWLINE also inserts \"* \" at the
beginning of each line. Comments are automagically created when you hit
\"\\M-;\". By default they go to column 48 (see variable sdf-comment-column),
but if a comment stands alone on a line it is indented as if it where a Seadif
statement.

The function sdf-where-am-i on \"\\C-c.\" prints in the minibuffer the
hierarchical path of the current position in the Seadif tree. The function
sdf-mark-sexp on \"\\M-\\C-h\" puts the region around the current Layout,
Circuit, Function or Library, whichever is nearest.  \(Warning: these functions
get confused if the cursor is inside a string delimited with \".\)

Other useful functions in Seadif mode that are also available in other modes:

\\M-\\C-f  (forward-sexp)     Move forward across one balanced expression.
\\M-\\C-b  (backward-sexp)    Move backward across one balanced expression.
\\M-\\C-u  (backward-up-list) Move backward out of one level of parentheses.
\\M-\\C-d  (down-list)        Move forward down one level of parentheses.
\\M-\\C-k  (kill-sexp)        Kill the syntactic expression following the cursor.
\\M-\\C-t  (transpose-sexps)  Interchange sexps around point, leaving point at
                            end of them.
\\M-\\C-\\  (indent-region)    Indent each nonblank line in the region. Warning:
                            the sdf-indent-line function was quickly written
                            but executes slowly (~ 50 lines/sec). You might
                            want to check with \\M-= how long it's gonna take
                            to indent the entire region.
\\C-x\\C-x (exchange-point-and-mark) Put the mark where point is now, and point
                            where the mark is now.

Summarizing the Seadif specific key-bindings:
\\{sdf-mode-map}
Turning on SEADIF mode calls the value of the variable sdf-mode-hook with no
args, if that value is non-nil.

The following variables control Seadif mode. You might want to change them in
your own sdf-mode-hook function:
sdf-indent-amount:    number of spaces to insert for each indent level (defaults
                      to 3).
sdf-align-paren:      if non-nil indent lines starting with ')' to the same
                      column as the corresponding '('. This is the default.
sdf-comment-column:   column to start /* ... */ comments (defaults to 48).
"
  (interactive)
  (kill-all-local-variables)
  (use-local-map sdf-mode-map)
  (setq major-mode 'sdf-mode)
  (setq mode-name "Seadif")
  (setq local-abbrev-table sdf-mode-abbrev-table)
  (set-syntax-table sdf-mode-syntax-table)
  (make-local-variable 'comment-start)
  (setq comment-start "/* ")
  (make-local-variable 'comment-end)
  (setq comment-end " */")
  (make-local-variable 'sdf-comment-column)
  (setq sdf-comment-column sdf-default-comment-column)
  (make-local-variable 'comment-start-skip)
  (setq comment-start-skip "/\\*+ *")
  (make-local-variable 'comment-indent-hook)
  (setq comment-indent-hook 'sdf-comment-indent-amount)
  (make-local-variable 'parse-sexp-ignore-comments)
  (setq parse-sexp-ignore-comments t)
  (make-local-variable 'indent-line-function)
  (setq indent-line-function 'sdf-indent-line)
  (run-hooks 'sdf-mode-hook))


;; This defines the relevant Seadif keywords. Case sensitive...:
(defvar sdf-lay "Layout")
(defvar sdf-cir "Circuit")
(defvar sdf-fun "Function")
(defvar sdf-lib "Library")
(defvar sdf-sdf "Seadif")


(defun sdf-indent-line (&optional no-align-paren)
       "Indent current Seadif line. Based on balance of parenthesis
        in previous line. See also the variable sdf-indent-amount.
        If optional argument NO-ALIGN-PAREN is non-nil, the function
        acts as if the global sdf-align-paren where nil."
       (interactive)
       (let (bol ind)
	    (save-excursion
	     ;; First, delete current indentation:
	     (beginning-of-line)
	     (setq bol (point))
	     (skip-chars-forward " \t")
	     (delete-region bol (point))
	     ;; Next, set local variable ind to the amount of indentation:
	     (setq ind (sdf-current-indent-column no-align-paren))
	     ;; Insert ind spaces at beginning of current line:
	     (while (> ind 0)
		    (insert " ")
		    (setq ind (1- ind)))
	     ;; Replace spaces with tabs as much as possible:
	     (tabify bol (point)))
	    ;; Sometimes save-excursion looses because we deleted the point
	    ;; where we started from. In that case, put cursor at indent column:
	    (if (equal (current-column) 0)
		(back-to-indentation))
	    ))


(defun sdf-current-indent-column (&optional no-align-paren)
       "Return the column at which current line should be indented. This
        looks at the indentation of the previous non-empty line and then adds
        the balance of parenthesis in that line, multiplied by sdf-indent-amount.
        If it looks like we're in a multi-line comment"
       (let ((align-paren (and (not no-align-paren) sdf-align-paren))
	     beg-of-this-line c (balance 0) (goal-column nil) indent-for-comment)
	    (save-excursion
	     (beginning-of-line)
	     (setq beg-of-this-line (point))
	     (and align-paren (looking-at "^[ \t]*)") (setq balance -1))
	     (sdf-beginning-of-previous-non-empty-line)
	     (and align-paren (looking-at "^[ \t]*)") (setq balance (1+ balance)))
	     (setq indent-for-comment	; check for multi-line comment...
		   (cond ((looking-at "^.*/\\*.*\\*/") 0) ; single line comment
			 ((and (looking-at "^[ \t]*/\\*")) 1) ; prev.line starts comment
			 ((looking-at "^.*\\*/") -1) ; prev line ends comment
			 (t 0)))
	     (if (equal (point) beg-of-this-line)
		 0			; no indent if this is first line in file
		 (skip-chars-forward " \t")
		 (setq current-depth (current-column))
		 (while (not (equal (setq c (char-after (point))) ?\n))
			(cond ((equal c ?\( ) (setq balance (1+ balance)))
			      ((equal c ?\) ) (setq balance (1- balance))))
			(goto-char (1+ (point))))
		 (+ current-depth (* balance sdf-indent-amount) indent-for-comment)
		 ))))


(defun sdf-beginning-of-previous-non-empty-line ()
       "Set point to beginning of first non-empty line before current point.
       Does not look back more than 500 chars."
       (skip-chars-backward " \t\n" (max 0 (- (point) 100)))
       (beginning-of-line))


(defun sdf-newline-and-indent ()
       "Insert a newline and then indent according to current Seadif expression.
        While editing a multi-line comment insert '* ' at beginning of new line."
       (interactive)
       (sdf-indent-line)
       (insert "\n")
       (let (continue-comment)
	    (save-excursion
	     (previous-line 1)
	     (beginning-of-line)
	     (setq continue-comment (and (looking-at "^[ \t]*/?\\*[^/]")
					 (not (looking-at ".*\\*/")))))
	    (delete-horizontal-space)
	    (if continue-comment (insert "* ")))
       (sdf-indent-line t))


(defun sdf-comment-indent-amount ()
       "Return the column where Seadif comment starts."
       (save-excursion
	(skip-chars-backward " \t")
	(if (equal (current-column) 0)	;if comment stands alone on a line
	    (sdf-current-indent-column)	;  indent it as if it where normal seadif code
	    (max sdf-comment-column	;Else indent at Seadif comment column
		 (1+ (current-column)))))) ;  except leave at least one space.


(defun sdf-mark-sexp ()
       "Put region around current Seadif expression."
       (interactive)
       (let* ((intro-chars "([ \\t\\n]*")
	      (start-of-sexp
	       (concat intro-chars "\\(" sdf-sdf "\\|" sdf-lay "\\|"
		       sdf-cir "\\|" sdf-fun "\\|" sdf-lib "\\)"))
	      (case-fold-search nil)
	      (start (point)))
	     (message "working...")
	     (backward-char 1)
	     (while (not (looking-at start-of-sexp))
		    (backward-up-list 1))
	     (if (looking-at (concat intro-chars sdf-sdf))
		 (progn (beep)
			(message "No higher Seadif level exists !!!")
			(goto-char start))
		 (setq start (point))
		 (forward-list 1)
		 (push-mark (point))
		 (goto-char start))))


(defun sdf-where-am-i ()
       "Print hierarchical name of the current Seadif object."
       (interactive)
       (save-excursion
	(let* ((intro-chars "([ \\t\\n]*")
	       (start-of-sexp
		(concat intro-chars "\\(" sdf-sdf "\\|" sdf-lay "\\|"
			sdf-cir "\\|" sdf-fun "\\|" sdf-lib "\\)"))
	       (case-fold-search nil)
	       lib-name fun-name cir-name lay-name)
	      (message "working...")
	      (forward-char 1)		;compensate for next backward-char...
	      (while (not lib-name)
		     (backward-char 1)
		     (while (not (looking-at start-of-sexp))
			    (backward-up-list 1))
		     (cond ((looking-at (concat intro-chars sdf-lay))
			    (setq lay-name (sdf-read-sexp-name)))
			   ((looking-at (concat intro-chars sdf-cir))
			    (setq cir-name (sdf-read-sexp-name)))
			   ((looking-at (concat intro-chars sdf-fun))
			    (setq fun-name (sdf-read-sexp-name)))
			   ((looking-at (concat intro-chars sdf-lib))
			    (setq lib-name (sdf-read-sexp-name)))
			   (t		; something strange going on... quit:
			    (beep)
			    (setq lib-name "**Strange kind 'a Library you've got...**")))
		     (sdf-print-name lay-name cir-name fun-name lib-name))
	      )))


(defun sdf-print-name (lay cir fun lib)
       "Print hierarchical Seadif name, showing \"...\" for unknown components."
       (let ((dots "..."))
	    (cond
	     (lay (message "%s (%s(%s(%s(%s))))" sdf-lay
			   lay (or cir dots) (or fun dots) (or lib dots)))
	     (cir (message "%s (%s(%s(%s)))" sdf-cir
			   cir (or fun dots) (or lib dots)))
	     (fun (message "%s  (%s(%s))" sdf-fun
			   fun (or lib dots)))
	     (lib (message "%s (%s)" sdf-lib
			   lib))
	     )))


(defun sdf-read-sexp-name ()
       "With point currently at the start of a Seadif sexp,
        return the name of the sexp as a string. If the name
        is quoted within \" then strip these quotes first."
       (let (bow quoted)
	    (save-excursion
	     (forward-word 1)		; skip Seadif keyword
	     (skip-chars-forward " \t\n")
	     (setq bow (point))
	     (setq quoted (looking-at "\""))
	     (forward-sexp 1)
	     (if quoted
		 (buffer-substring (1+ bow) (1- (point)))
		 (buffer-substring bow (point))))))

(defun sdf-undefined ()
       "Bind undesired key sequences to this function."
       (interactive)
       (beep)
       (message "this key sequence was disabled for your own health..."))
