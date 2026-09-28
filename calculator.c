#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 500

// ================= STACK FOR OPERATORS (CHAR) =================
typedef struct {
    char items[MAX];
    int top;
} CharStack;

void initCharStack(CharStack *s) { s->top = -1; }
int isCharEmpty(CharStack *s) { return s->top == -1; }
void pushChar(CharStack *s, char c) { s->items[++(s->top)] = c; }
char popChar(CharStack *s) { return s->items[(s->top)--]; }
char peekChar(CharStack *s) { return s->items[s->top]; }

// ================= STACK FOR NUMBERS (DOUBLE) =================
typedef struct {
    double items[MAX];
    int top;
} NumStack;

void initNumStack(NumStack *s) { s->top = -1; }
int isNumEmpty(NumStack *s) { return s->top == -1; }
void pushNum(NumStack *s, double val) { s->items[++(s->top)] = val; }
double popNum(NumStack *s) { return s->items[(s->top)--]; }
double peekNum(NumStack *s) { return s->items[s->top]; }

// Custom power function (taaki math.h ya -lm ki zaroorat na pade)
double customPow(double base, double exp) {
    if (exp == 0) return 1.0;
    int isNegative = (exp < 0);
    long n = (long)(isNegative ? -exp : exp);
    double result = 1.0;
    while (n > 0) {
        if (n % 2 == 1) result *= base;
        base *= base;
        n /= 2;
    }
    return isNegative ? (1.0 / result) : result;
}

// Custom modulo for floating numbers
double customMod(double a, double b) {
    if (b == 0) return 0;
    long quotient = (long)(a / b);
    return a - (quotient * b);
}

// Operator Precedence
int precedence(char op) {
    switch (op) {
        case '+':
        case '-': return 1;
        case '*':
        case '/':
        case '%': return 2;
        case '^': return 3;
        default:  return 0;
    }
}

int isRightAssociative(char op) {
    return (op == '^');
}

int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '^');
}

// ================= STEP 1: INFIX TO POSTFIX =================
void infixToPostfix(const char *infix, char *postfix) {
    CharStack opStack;
    initCharStack(&opStack);
    int j = 0;

    printf("\n========================================================\n");
    printf("   STEP 1: INFIX TO POSTFIX CONVERSION (USING STACK)    \n");
    printf("========================================================\n");
    printf("%-10s | %-20s | %-20s\n", "Symbol", "Operator Stack", "Postfix Buffer");
    printf("--------------------------------------------------------\n");

    for (int i = 0; infix[i] != '\0'; i++) {
        char ch = infix[i];

        if (isspace(ch)) continue;

        if (isdigit(ch) || ch == '.') {
            while (isdigit(infix[i]) || infix[i] == '.') {
                postfix[j++] = infix[i++];
            }
            postfix[j++] = ' ';
            i--;
        } 
        else if (ch == '(') {
            pushChar(&opStack, ch);
        } 
        else if (ch == ')') {
            while (!isCharEmpty(&opStack) && peekChar(&opStack) != '(') {
                postfix[j++] = popChar(&opStack);
                postfix[j++] = ' ';
            }
            if (!isCharEmpty(&opStack)) {
                popChar(&opStack);
            }
        } 
        else if (isOperator(ch)) {
            while (!isCharEmpty(&opStack) && peekChar(&opStack) != '(' &&
                   (precedence(peekChar(&opStack)) > precedence(ch) ||
                   (precedence(peekChar(&opStack)) == precedence(ch) && !isRightAssociative(ch)))) {
                postfix[j++] = popChar(&opStack);
                postfix[j++] = ' ';
            }
            pushChar(&opStack, ch);
        }

        // Print step trace
        char stackState[MAX] = "";
        for (int k = 0; k <= opStack.top; k++) stackState[k] = opStack.items[k];
        stackState[opStack.top + 1] = '\0';
        
        char currentPost[MAX];
        strncpy(currentPost, postfix, j);
        currentPost[j] = '\0';

        char symbolStr[10];
        snprintf(symbolStr, sizeof(symbolStr), "%c", ch);
        printf("%-10s | %-20s | %-20s\n", symbolStr, stackState, currentPost);
    }

    while (!isCharEmpty(&opStack)) {
        postfix[j++] = popChar(&opStack);
        postfix[j++] = ' ';
    }
    postfix[j] = '\0';

    printf("--------------------------------------------------------\n");
    printf("Generated Postfix Expression: %s\n", postfix);
}

// ================= STEP 2: POSTFIX EVALUATION =================
double evaluatePostfix(const char *postfix) {
    NumStack valStack;
    initNumStack(&valStack);

    printf("\n========================================================\n");
    printf("        STEP 2: POSTFIX EVALUATION (USING STACK)        \n");
    printf("========================================================\n");

    for (int i = 0; postfix[i] != '\0'; i++) {
        if (isspace(postfix[i])) continue;

        if (isdigit(postfix[i]) || (postfix[i] == '.' && isdigit(postfix[i + 1]))) {
            char numBuf[64];
            int k = 0;
            while (isdigit(postfix[i]) || postfix[i] == '.') {
                numBuf[k++] = postfix[i++];
            }
            numBuf[k] = '\0';
            double val = atof(numBuf);
            pushNum(&valStack, val);
            printf("Pushed Number to Stack : %8.2f\n", val);
            i--;
        } 
        else if (isOperator(postfix[i])) {
            char op = postfix[i];
            if (valStack.top < 1) {
                printf("\nError: Invalid mathematical expression format.\n");
                return 0.0;
            }
            double val2 = popNum(&valStack);
            double val1 = popNum(&valStack);
            double res = 0.0;

            switch (op) {
                case '+': res = val1 + val2; break;
                case '-': res = val1 - val2; break;
                case '*': res = val1 * val2; break;
                case '/':
                    if (val2 == 0) {
                        printf("\nMath Error: Division by zero not allowed.\n");
                        return 0.0;
                    }
                    res = val1 / val2;
                    break;
                case '%': res = customMod(val1, val2); break;
                case '^': res = customPow(val1, val2); break;
            }

            pushNum(&valStack, res);
            printf("Applied '%c' on (%.2f, %.2f) -> Pushed Result: %.2f\n", op, val1, val2, res);
        }
    }

    return popNum(&valStack);
}

// ================= MAIN FUNCTION =================
int main() {
    char infix[MAX];
    char postfix[MAX];
    char choice;

    do {
        printf("\n========================================================\n");
        printf("       DATA STRUCTURES: EXPRESSION CALCULATOR           \n");
        printf("========================================================\n");
        printf("Supports: +, -, *, /, %%, ^ (power), parentheses (), decimals\n");
        printf("Enter mathematical expression (e.g., (12 + 3.5) * 2 ^ 3): ");

        if (!fgets(infix, sizeof(infix), stdin)) break;
        
        // Clean line breaks (\n and \r for Windows/Linux portability)
        infix[strcspn(infix, "\r\n")] = 0;

        if (strlen(infix) == 0) {
            printf("Empty input entered. Exiting.\n");
            break;
        }

        postfix[0] = '\0';
        infixToPostfix(infix, postfix);
        double result = evaluatePostfix(postfix);

        printf("\n========================================================\n");
        printf("FINAL RESULT : %g\n", result);
        printf("========================================================\n");

        printf("\nDo you want to evaluate another expression? (y/n): ");
        scanf(" %c", &choice);
        while (getchar() != '\n'); 
    } while (choice == 'y' || choice == 'Y');

    printf("\nProject execution completed.\n");
    return 0;
}