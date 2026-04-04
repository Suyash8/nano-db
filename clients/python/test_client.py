from nanodb_client import NanoDBClient
import time

def main():
    print("Connecting to NanoDB...")
    client = NanoDBClient()
    
    print("Inserting data...")
    client.insert(2000, 3.14)
    client.insert(2001, 2.71)
    
    print("Querying data...")
    data = client.query(2000, 2005)
    print(f"Received: {data}")
    
    client.close()

if __name__ == "__main__":
    main()