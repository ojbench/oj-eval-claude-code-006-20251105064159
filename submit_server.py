#!/usr/bin/env python3
import requests
import json
import os

# Read the server.h file
with open('src/include/server.h', 'r') as f:
    code_text = f.read()

# Prepare submission data
data = {
    "language": "cpp",
    "code": code_text
}

# Make the submission request
headers = {
    "Authorization": f"Bearer {os.environ['ACMOJ_TOKEN']}",
    "Content-Type": "application/x-www-form-urlencoded"
}

response = requests.post(
    "https://acm.sjtu.edu.cn/OnlineJudge/api/v1/problem/2876/submit",
    headers=headers,
    data=data
)

if response.status_code == 200:
    result = response.json()
    print(json.dumps(result, indent=2))

    # Save submission ID
    if 'id' in result:
        submission_id = result['id']
        print(f"\nSubmission ID: {submission_id}")

        # Save to log file
        with open('/workspace/submission_ids.log', 'a') as f:
            f.write(f"{submission_id}\n")
else:
    print(f"Submission failed with status code: {response.status_code}")
    print(f"Response: {response.text}")