import subprocess
import random

def generate_test_case():
    """Generuje losowy przypadek testowy zgodnie z wymaganiami."""
    t = random.randint(1, 16)
    d = random.randint(3, 28)
    n = random.randint(0, 5)
    m = random.randint(0, 5)
    
    A0 = [random.randint(1, d) for _ in range(n)]
    B0 = [random.randint(1, d) for _ in range(m)]
    
    input_data = f"{t} {d} {n} {m}\n"
    input_data += " ".join(map(str, A0)) + "\n"
    input_data += " ".join(map(str, B0)) + "\n"
    
    return input_data

def run_program(program, input_data):
    """Uruchamia program i zwraca jego wyjście."""
    process = subprocess.run(
        program, 
        input=input_data, 
        text=True, 
        capture_output=True
    )
    return process.stdout.strip(), process.stderr.strip()

def test_parallel_against_reference(num_tests=1000):
    """Testuje `parallel` przeciwko `reference`, porównując tylko pierwszą linię wyjścia."""
    for i in range(num_tests):
        # Generowanie testu
        input_data = generate_test_case()
        
        # Uruchamianie programów
        parallel_output, parallel_error = run_program('./build/parallel/parallel', input_data)
        reference_output, reference_error = run_program('./build/reference/reference', input_data)
        
        # Porównanie tylko pierwszej linii wyjścia
        parallel_first_line = parallel_output.splitlines()[0] if parallel_output else ""
        reference_first_line = reference_output.splitlines()[0] if reference_output else ""
        
        if parallel_first_line != reference_first_line:
            print(f"Test {i+1} failed.")
            print("Input:")
            print(input_data)
            print("\nExpected First Line:")
            print(reference_first_line)
            print("\nParallel First Line:")
            print(parallel_first_line)
            print("\nErrors:")
            print(f"Parallel: {parallel_error}")
            print(f"Reference: {reference_error}")
            return
        
        print(f"Test {i+1} passed.")
    
    print("All tests passed.")

if __name__ == "__main__":
    test_parallel_against_reference(num_tests=1000)

