export BASE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

export PATH=$PATH:"${BASE_DIR}/tools"

export YEAR=$(cat ${BASE_DIR}/year)
export SESSION_KEY=$(cat ${BASE_DIR}/session_key)

echo "Alright, ready for year ${YEAR} ;-)"
